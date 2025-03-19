#include <iostream>
#include <string>
#include "DataEngine.h"
#include "Contact.h"
#ifdef BUILD_QT
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#endif
class Callback : public DataEngine::DatabaseCallback
{
    public:
        void onDatabaseChanged(const std::string& dbName, const int& id , const std::shared_ptr<Contact>& contact, const DataEngine::Action& action) override
        {
            std::cout << "main onDatabaseChanged : " << dbName << " action: " << static_cast<int>(action) << " \n";

        }
};

int main(int argc, char *argv[])
{
  #ifdef BUILD_QT

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    DataEngine& instance = DataEngine::getInstance();
    HmiInterface hmiIntf;
    // engine.rootContext()->setContextProperty("interface", &interface);
    // qmlRegisterType<HmiInterface>("MyApp", 1, 0, "HmiInterface");
    engine.rootContext()->setContextProperty("hmiIntf", &hmiIntf);
    engine.clearComponentCache();

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
  #elif
    DataEngine& instance = DataEngine::getInstance();
    std::string input;
    Callback callbackIml;
    std::thread* callbackThread = new std::thread([&instance, &callbackIml](){
        instance.registerCallback(&callbackIml);
    });

    while (true)
    {
      std::cout << "***************************************************************** \n";
      std::cout << "1. Add \n";
      std::cout << "2. Get dump \n";
      std::cout << "3. Delete \n";
      std::cout << "***************************************************************** \n";
      std::cout << "Please typing the number which indicate your choice: ";
      std::getline(std::cin, input);
      if (input == "1") {
        std::cout << "1. Add by information\n";
        std::cout << "2. Add by contact\n";
        std::getline(std::cin, input);
        if(input == "1"){
          instance.addContact("Duy", {"0865950091", "0909090909"}, "My num", "/home/duynp/C++/SqlLite/build/meo.jpg");
          instance.addContact("Huy", {"0999999999"}, "My num", "/home/duynp/C++/SqlLite/build/meo.jpg");
          instance.addContact("Duy1", {"0865950091"}, "My num", "/home/duynp/C++/SqlLite/build/meo.jpg");
          instance.addContact("Huy1", {"0999999999"}, "My num", "/home/duynp/C++/SqlLite/build/meo.jpg");
          instance.addContact("Duy2", {"0865950091"}, "My num", "/home/duynp/C++/SqlLite/build/meo.jpg");
          instance.addContact("HuyGay", {"0999999999"}, "My num", "/home/duynp/C++/SqlLite/build/meo.jpg");
        }
        else{
          std::shared_ptr<Contact> contact = Contact::Builder().setName("Huy").setPhoneNumbers({"0384988891"}).setNotes("Mynum").setImageUri("/home/duynp/C++/SqlLite/build/meo.jpg").buildShared();
          instance.addContact(*contact);
        }
      }
      else if (input == "2") {
        instance.dump();
      }
      else if(input == "3") {
        std::cout << "Enter id of contact: ";
        std::getline(std::cin, input);
        int id = stoi(input);
        instance.deleteContact(id);
      }
    } 
  #endif

    
}