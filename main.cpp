#include <iostream>
#include <string>
#include "DataEngine.h"
#include "Contact.h"

class Callback : public DataEngine::DatabaseCallback
{
    public:
        void onDatabaseChanged(const std::string dbName, const DataEngine::Action& action ,const int& id, const std::shared_ptr<Contact>& contact) override
        {
            std::cout << "main onDatabaseChanged : " << dbName << " action: " << static_cast<int>(action) << " \n";

        }
};

int main(void)
{
    DataEngine& instance = DataEngine::getInstance();
    std::string input;
    Callback callbackIml;
    std::thread* callbackThread = new std::thread([&instance, &callbackIml](){
        instance.registerCallback(&callbackIml);
    });
    while (true)
    {
      std::cout << "Enter Message: ";
      std::getline(std::cin, input);
      if (input == "add") {
        instance.addContact("Duy", {"0865950091"}, "My num", "/home/duynp/C++/SqlLite/build/meo.jpg");
        instance.addContact("Huy", {"0999999999"}, "My num", "/home/duynp/C++/SqlLite/build/meo.jpg");
      }
      else if (input == "dump") {
        instance.dump();
      }else if(input == "del") {
        std::cout << "Enter id of contact: ";
        std::getline(std::cin, input);
        int id = stoi(input);
        instance.deleteContact(id);
      }
    } 
}