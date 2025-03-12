#include "DataEngine.h"

static DataEngine* gInstance = nullptr;

static const std::string DB_NAME = "PhoneContact.db";

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

bool DataEngine::addContact(const std::string& name, const std::vector<std::string>& numbers, const std::string& notes, const std::string& uri)
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

                try
                {
                    sqlite3_bind_text(stmt, 1, contact->getName().c_str(), -1, SQLITE_STATIC);
                    sqlite3_bind_text(stmt, 2, contact->getPhoneNumbersString().c_str(), -1, SQLITE_STATIC);
                    sqlite3_bind_blob(stmt, 3, contact->getblobImage().data(), contact->getblobImage().size(), SQLITE_STATIC);
                    sqlite3_bind_text(stmt, 4, contact->getNotes().c_str(), -1, SQLITE_STATIC);
                    result = sqlite3_step(stmt);
                    checkOperation(result, " inserting " );
                    int id = sqlite3_last_insert_rowid(mDatabase);
                    sqlite3_finalize(stmt);
                    notifyCallback(DB_NAME, id, contact);
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
                
            }
        });
        mQueueCV.notify_all();
    }
}

bool DataEngine::addContact(const Contact& contact)
{

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

void DataEngine::notifyCallback(const std::string dbName, const int& id , const std::shared_ptr<Contact>& contact)

{

}

