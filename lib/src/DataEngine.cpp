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

    int result = sqlite3_open(DB_NAME.c_str(), &mDatabase);
    checkOperation(result, " open database ");

    result = sqlite3_exec(mDatabase, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    checkOperation(result, " exec database in WAL mode");

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
    
    mWorkerThread = new std::thread(&DataEngine::workerThread, this);
    sqlite3_update_hook(mDatabase, onUpdateHook, this);
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

void DataEngine::checkOperation(int ret, const std::string& message)
{
    switch (ret)
    {
        case SQLITE_OK: {
            [[fallthrough]];
        }
        case SQLITE_DONE: {
            break;
        }
        default: {
            throw std::runtime_error("SQLite error during " + message + ": " + sqlite3_errmsg(mDatabase));
            break;
        }
    }
}

void DataEngine::addContact(const std::string& name, const std::vector<std::string>& numbers, const std::string& notes, const std::string& uri)
{
    {
        std::lock_guard<std::mutex> lock(mQueueMutex);
        mTaskQueue.emplace([name, numbers, notes, uri, this]() {
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
                    sqlite3_bind_text(stmt, 1, contact->getName().c_str(), -1, SQLITE_STATIC);
                    sqlite3_bind_text(stmt, 2, contact->getPhoneNumbersString().c_str(), -1, SQLITE_STATIC);
                    sqlite3_bind_blob(stmt, 3, contact->getblobImage().data(), contact->getblobImage().size(), SQLITE_STATIC);
                    sqlite3_bind_text(stmt, 4, contact->getNotes().c_str(), -1, SQLITE_STATIC);

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

std::vector<unsigned char> DataEngine::loadJPEG(const std::string& filePath)
{

}

void DataEngine::workerThread()
{

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
    instance->mTaskQueue.emplace([instance, rowId, contact, action](){
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
