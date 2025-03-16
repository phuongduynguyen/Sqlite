#ifndef DATAENGINE_H
#define DATAENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <stdexcept>
#include <sqlite3.h>
#include <memory>
#include <queue>
#include <condition_variable>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include <list>
#include "Contact.h"

class DataEngine
{
    public:
        static DataEngine& getInstance();

        enum class Action
        {
            None,
            Insert,
            Update,
            Delete
        };
        class DatabaseCallback {
            public:
                virtual void onDatabaseChanged(const std::string dbName, const DataEngine::Action& action ,const int& id, const std::shared_ptr<Contact>& contact) = 0;
        };

        void addContact(const std::string& name, const std::vector<std::string>& numbers, const std::string& notes, const std::string& uri);
        bool addContact(const Contact& contact);
        bool updateContact(const int& id, const Contact& contact);
        bool deleteContact(const int& id);
        bool deleteContact(const Contact& contact);
        bool isExistContact(const int& id);
        bool isDatabaseEmpty();
        void resetIDCounter();
        void updateID(const int& id);
        std::string getNameFromID(const int& id);
        std::vector<std::shared_ptr<Contact>> searchByName(const std::string& name);
        std::vector<std::shared_ptr<Contact>> searchByNumber(const std::string& number);
        void registerCallback(DataEngine::DatabaseCallback* callback);
        void dump();
        
    private:
        void syncData(); 
        std::vector<unsigned char> loadJPEG(const std::string& filePath);
        void workerThread();
        void openDatabase(const std::string& dbPath);
        void notifyCallback(const std::string& dbName, const int& id , const std::shared_ptr<Contact>& contact, const DataEngine::Action& action);
        void checkOperation(int ret, const std::string& message);
        static void onUpdateHook(void* userData, int operation, const char* databaseName, const char* tableName, sqlite3_int64 rowId);

        DataEngine();
        ~DataEngine();

        sqlite3* mDatabase;
        std::mutex mMutex;
        std::list<std::function<void()>> mTaskQueue;
        std::mutex mQueueMutex;
        std::condition_variable mQueueCV;
        std::thread* mWorkerThread;
        bool mRunning;
        std::vector<DataEngine::DatabaseCallback*> mCallbacks;
        std::vector<std::shared_ptr<Contact>> mContacts;
        std::unordered_map<std::string,std::shared_ptr<Contact>> mContactsTable;
};
#endif // DATAENGINE_H

