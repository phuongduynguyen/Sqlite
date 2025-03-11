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
#include "Contact.h"
class DataEngine
{
    public:
        static DataEngine& getInstance();

        class DatabaseCallback {
            public:
                virtual void onDatabaseChanged(const std::string dbName, const Contact& contact) = 0;
        };

        bool addContact(const std::string& name, const std::vector<std::string>& numbers, const std::string& notes, const std::string& uri);
        bool addContact(const Contact& contact);
        bool updateContact(const int& id, const Contact& contact);
        bool deleteContact(const int& id);
        bool deleteContact(const Contact& contact);
        std::vector<std::shared_ptr<Contact>> searchByName(const std::string& name);
        std::vector<std::shared_ptr<Contact>> searchByNumber(const std::string& number);
        void registerCallback(DataEngine::DatabaseCallback* callback);

    private:
        std::vector<unsigned char> loadJPEG(const std::string& filePath);
        void workerThread();
        void openDatabase(const std::string& dbPath);
        void notifyCallback(const std::string dbName, const Contact& contact);
        void checkOperation(int ret, const std::string& message);

        DataEngine();
        ~DataEngine();
        sqlite3* mDatabase;
        std::mutex mMutex;
        std::queue<std::function<void()>> mTaskQueue;
        std::mutex mQueueMutex;
        std::condition_variable mQueueCV;
        std::thread* mWorkerThread;
        bool mRunning;
        std::vector<std::function<void(const std::string&, const Contact&)>> mCallbacks;
        std::vector<std::shared_ptr<Contact>> mContacts;
};
#endif // DATAENGINE_H

