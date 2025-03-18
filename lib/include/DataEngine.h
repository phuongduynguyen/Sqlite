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
                virtual void onDatabaseChanged(const std::string& dbName, const int& id , const std::shared_ptr<Contact>& contact, const DataEngine::Action& action) = 0;
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

        template<typename T>
        void registerCallback(T&& callback) {
            {
                std::lock_guard<std::mutex> lock(mMutex);
                if (!std::is_convertible_v<std::remove_pointer_t<std::decay_t<T>>*, DataEngine::DatabaseCallback*>) {
                    std::wcerr << "registerCallback with invalid type \n";
                    return;
                }
        
                std::function<void(const std::string& , const int& , const std::shared_ptr<Contact>& , const DataEngine::Action& )> callbackFunc = [cb = std::forward<T>(callback)](const std::string& dbName, const int& id , const std::shared_ptr<Contact>& contact, const DataEngine::Action& action) {
                    if constexpr ((std::is_pointer_v<std::decay_t<T>>) || (std::is_same_v<std::decay_t<T>, std::shared_ptr<DataEngine::DatabaseCallback>>)) {
                        if (cb != nullptr) {
                            cb->onDatabaseChanged(dbName, id, contact, action);
                        }
                        else {
                            std::wcerr << "onDatabaseChanged with invalid cb \n";
                        }
                    }
                };
                mCallbacks.emplace_back(callbackFunc);
            }
        }

        void dump();

    private:
        void syncData(); 
        std::vector<unsigned char> loadJPEG(const std::string& filePath);
        void workerThread();
        void watcherThread();
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
        std::thread* mWatcherThread;
        bool mRunning;
        std::vector<std::function<void(const std::string& dbName, const int& id , const std::shared_ptr<Contact>& contact, const DataEngine::Action& action)>> mCallbacks;
        std::vector<std::shared_ptr<Contact>> mContacts;
        std::unordered_map<std::string,std::shared_ptr<Contact>> mContactsTable;
};
#endif // DATAENGINE_H

