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
    if (SQLITE_OK != ret) {
        throw std::runtime_error("SQLite error during " + message + ": " + sqlite3_errmsg(mDatabase));
    }
    
}

bool DataEngine::addContact(const std::string& name, const std::vector<std::string>& numbers, const std::string& notes, const std::string& uri)
{

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

void DataEngine::notifyCallback(const std::string dbName, const Contact& contact)
{

}

