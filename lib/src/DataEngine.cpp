#include "DataEngine.h"

static DataEngine* gInstance = nullptr;

static const std::string DB_NAME = "PhoneContact.db";

std::ostream& operator<<(std::ostream& strm, const DataEngine::Action& values);

DataEngine& DataEngine::getInstance()
{
    if (gInstance == nullptr) {
        gInstance = new DataEngine();
    }
    return *gInstance;
}

DataEngine::DataEngine()
{
    if (!std::filesystem::exists(DB_NAME)) {
        std::cout << "Database " << DB_NAME << " not exists, creating .., \n";
    }

    int result = sqlite3_open_v2(DB_NAME.c_str(), &mDatabase, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
    checkOperation(result, "Open database ");

    result = sqlite3_exec(mDatabase, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    checkOperation(result, "Exec database in WAL mode");

    result =  sqlite3_exec(mDatabase, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, nullptr);
    checkOperation(result, "Exec database in PRAGMa ");

    const char* createTableSQL = 
            "CREATE TABLE IF NOT EXISTS contacts ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT, "
            "phone TEXT NOT NULL, "
            "photo BLOB, "
            "notes TEXT);"
            "CREATE INDEX IF NOT EXISTS idx_name ON contacts(name);"
            "CREATE INDEX IF NOT EXISTS idx_phone ON contacts(phone);";

    char* errMsg = nullptr;
    result = sqlite3_exec(mDatabase, createTableSQL, nullptr, nullptr, &errMsg);
    if (result != SQLITE_OK) {
        std::string error(errMsg);
        sqlite3_free(mDatabase);
        throw std::runtime_error("Failed to create table: " + error);
    }
    mRunning = true;
    mWorkerThread = new std::thread(&DataEngine::workerThread, this);
    sqlite3_update_hook(mDatabase, onUpdateHook, this);
    syncData();
}

DataEngine::~DataEngine()
{
    {
        std::lock_guard<std::mutex> lock(mQueueMutex);
        mRunning = false;
        mQueueCV.notify_all();
    }

    if (mWorkerThread->joinable()) {
        mWorkerThread->join();
        delete mWorkerThread;
        mWorkerThread = nullptr;
    }

    if (mDatabase != nullptr) {
        sqlite3_close(mDatabase);
    }
    
}

void DataEngine::syncData()
{
    std::cout << "Loading existing data from database...\n";
    {
        std::lock_guard<std::mutex> lock(mMutex);
        sqlite3_stmt* stmt;
        const char* query = "SELECT name, phone, photo, notes FROM contacts";
        int result = sqlite3_prepare_v2(mDatabase, query, -1, &stmt, nullptr);
        checkOperation(result, "syncData prepare");
    
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            std::string name = sqlite3_column_text(stmt, 0) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)) : "";
            std::string phone = sqlite3_column_text(stmt, 1) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) : "";
            const void* photoBlob = sqlite3_column_blob(stmt, 2);
            int photoSize = sqlite3_column_bytes(stmt, 2);
            std::vector<unsigned char> photo;
            if ((photoBlob != nullptr) && (photoSize > 0)) {
                photo.assign(static_cast<const unsigned char*>(photoBlob), static_cast<const unsigned char*>(photoBlob) + photoSize);
            } 
            else {
                photo = std::vector<unsigned char>();
            }
            std::string notes = sqlite3_column_text(stmt, 3) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "";
            std::shared_ptr<Contact> contact = Contact::Builder().setName(name).setPhoneNumbers({phone}).setBlobImage(photo).setNotes(notes).buildShared();
            mContactsTable.emplace(name, contact);
        }
    }
}

void DataEngine::checkOperation(int ret, const std::string& message)
{
    switch (ret)
    {
        case SQLITE_OK: {
            [[fallthrough]];
        }
        case SQLITE_DONE: {
            std::cout << message << " success\n";
            break;
        }
        default: {
            throw std::runtime_error("SQLite error during " + message + ": " + sqlite3_errmsg(mDatabase));
            break;
        }
    }
}

static void debugStmt(sqlite3_stmt* stmt) {
    if (!stmt) {
        std::cout << "Stmt is null\n";
        return;
    }
    std::cout << "Debugging sqlite3_stmt:\n";
    std::cout << "  - Parameter count: " << sqlite3_bind_parameter_count(stmt) << "\n";
    std::cout << "  - SQL: " << sqlite3_sql(stmt) << "\n";
    const char* expandedSql = sqlite3_expanded_sql(stmt);
    if (expandedSql) {
        std::cout << "  - Expanded SQL: " << expandedSql << "\n";
        sqlite3_free((void*)expandedSql);
    } else {
        std::cout << "  - Expanded SQL not available (SQLite version too old or no bindings yet)\n";
    }
}

void DataEngine::addContact(const std::string& name, const std::vector<std::string>& numbers, const std::string& notes, const std::string& uri)
{
    {
        std::lock_guard<std::mutex> lock(mQueueMutex);
        mTaskQueue.emplace_back([name, numbers, notes, uri, this]() {
            {
                std::lock_guard<std::mutex> lock(mMutex);
                const char* insertSQL = "INSERT INTO contacts (name, phone, photo, notes) VALUES (?, ?, ?, ?);";
                sqlite3_stmt* stmt;
                int result = sqlite3_prepare_v2(mDatabase, insertSQL, -1, &stmt, nullptr);
                checkOperation(result, " prepare insert ");

                if(mContactsTable.find(name) != mContactsTable.end()) {
                    std::cout << "Contact exists, dont need add \n";
                    return;
                }

                std::shared_ptr<Contact> contact = Contact::Builder().setName(name).setPhoneNumbers(numbers).setNotes(notes).setImageUri(uri).buildShared();
                mContactsTable.emplace(name,contact);
                try {
                    std::string name = contact->getName();
                    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
                    std::string phone = contact->getPhoneNumbersString();
                    sqlite3_bind_text(stmt, 2, phone.c_str(), -1, SQLITE_STATIC);
                    if (contact->getblobImage().size() == 0) {
                        sqlite3_bind_blob(stmt, 3, contact->getblobImage().data(), contact->getblobImage().size(), SQLITE_STATIC);
                    }
                    else {
                        sqlite3_bind_null(stmt, 3);
                    }
                    sqlite3_bind_text(stmt, 4, contact->getNotes().c_str(), -1, SQLITE_STATIC);
                    debugStmt(stmt);
                    result = sqlite3_step(stmt);
                    checkOperation(result, " inserting " );

                    int id = sqlite3_last_insert_rowid(mDatabase);
                    sqlite3_finalize(stmt);
                    notifyCallback(DB_NAME, id, contact, DataEngine::Action::Insert);
                }
                catch(const std::exception& e) {
                    std::cerr << e.what() << '\n';
                }
            }
        });
        mQueueCV.notify_all();
    }
}

bool DataEngine::addContact(const Contact& contact)
{
    {
        std::lock_guard<std::mutex> lock(mQueueMutex);
    }
}

bool DataEngine::updateContact(const int& id, const Contact& contact)
{

}

bool DataEngine::deleteContact(const int& id)
{

}

bool DataEngine::deleteContact(const Contact& contact)
{

}

std::vector<std::shared_ptr<Contact>> DataEngine::searchByName(const std::string& name)
{

}

std::vector<std::shared_ptr<Contact>> DataEngine::searchByNumber(const std::string& number)
{

}

void DataEngine::registerCallback(DataEngine::DatabaseCallback* callback)
{

}

void DataEngine::dump()
{
    std::unordered_map<std::string,std::shared_ptr<Contact>>::iterator item = mContactsTable.begin();
    while (item != mContactsTable.end())
    {
        std::cout << item->second->toString();
        item++;
    }
}

std::vector<unsigned char> DataEngine::loadJPEG(const std::string& filePath)
{

}

void DataEngine::workerThread()
{
    while(mRunning)
    {
        std::list<std::function<void()>> taskQueue;
        {
            std::unique_lock<std::mutex> lock(mQueueMutex);
            mQueueCV.wait(lock,[this](){
                return !mTaskQueue.empty();
            });
            taskQueue = mTaskQueue;
            mTaskQueue.clear();
        }

        while (!taskQueue.empty())
        {
            std::function<void()> task = std::move(taskQueue.front());
            taskQueue.pop_front();
            task();
        }
    }
}

void DataEngine::openDatabase(const std::string& dbPath)
{

}

void DataEngine::notifyCallback(const std::string& dbName, const int& id , const std::shared_ptr<Contact>& contact, const DataEngine::Action& action)
{

}

void DataEngine::onUpdateHook(void* userData, int operation, const char* databaseName, const char* tableName, sqlite3_int64 rowId)
{
    if (!userData) {
        std::cout << "onUpdateHook fail due to null userData\n";
        return;
    }
    std::string name = "";
    std::string phone = "";
    const void* photo = "";
    std::string notes = "";

    DataEngine* instance = static_cast<DataEngine*>(userData);
    DataEngine::Action action = DataEngine::Action::None;
    sqlite3_stmt* stmt;
    std::string query = "SELECT name, phone, photo, notes FROM " + std::string(tableName) + " WHERE rowid = ?;";
    if (sqlite3_prepare_v2(instance->mDatabase, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, rowId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            photo = reinterpret_cast<const char*>(sqlite3_column_blob(stmt, 2));
            notes = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            std::cout << "Affected Contact: Name=" << name << ", Phone=" << phone << ", Photo=" << photo << ", Notes=" << notes << "\n";
        }
        sqlite3_finalize(stmt);
    }
    
    std::shared_ptr<Contact> contact;
    switch (operation)
    {
        case SQLITE_INSERT: {
            action = DataEngine::Action::Insert;
            if (instance->mContactsTable.find(name) == instance->mContactsTable.end()) {
                contact = Contact::Builder().setName(name).setPhoneNumbers({phone}).setNotes(notes).buildShared();
                instance->mContactsTable.emplace(name,contact);
            }
            break;
        }
        case SQLITE_UPDATE: {
            action = DataEngine::Action::Update;
            std::unordered_map<std::string, std::shared_ptr<Contact>>::iterator foundedItem = instance->mContactsTable.find(name);
            if (foundedItem != instance->mContactsTable.end()) {
                contact = foundedItem->second;

                if (name != contact->getName()){
                    std::string oldName = contact->getName();
                    contact->setName(name);
                    instance->mContactsTable.erase(oldName);
                    instance->mContactsTable.emplace(name,contact);
                }

                contact->setNotes(notes);
                contact->setPhoneNumbers({phone});
                // contact->setUri("");
            }
            break;
        }
        case SQLITE_DELETE: {
            action = DataEngine::Action::Delete;
            std::unordered_map<std::string, std::shared_ptr<Contact>>::iterator foundedItem = instance->mContactsTable.find(name);
            if(foundedItem != instance->mContactsTable.end()) {
                contact = foundedItem->second;
                instance->mContactsTable.erase(name);
            }
            break;
        }
        default: {
            break;
        }
    }
    std::cout << "onUpdateHook action: " << action << " Database: " << databaseName << " tableName: " << tableName << " rowId: " << rowId << "\n";
    instance->mTaskQueue.emplace_back([instance, rowId, contact, action](){
        instance->notifyCallback(DB_NAME, rowId, contact, action);
    });
}

std::ostream& operator<<(std::ostream& strm, const DataEngine::Action& action)
{
    std::ostream *ptr = &strm;
    static const char * valueTbl[] = {
        "None"
        "Insert",
        "Update",
        "Delete",
    };
    uint32_t type = static_cast<uint32_t>(action);
    if (type > static_cast<uint32_t>(3)) {
        strm << "Unknown";
    }
    else {
        strm << valueTbl[type];
    }
    return *ptr;
}
