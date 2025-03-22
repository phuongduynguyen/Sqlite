
#include "HmiInterface.h"

void HmiInterface::sendMessage(const QString &msg)
{
    qDebug() << "sendMessage " << msg;
}

void HmiInterface::addContactInterface(const QString& name, const std::vector<QString>& numbers, const QString& notes, const QString& uri)
{
    qDebug() << "Add contact from HMI";
    std::vector<std::string> phone;
    
    for (const QString &numbers : numbers) {
        phone.push_back(numbers.toStdString());
    }

    mInstance.addContact(name.toStdString(),phone,notes.toStdString(),uri.toStdString());
}

void HmiInterface::deleteContactInterface(const QString& id)
{
    qDebug() << "Delete contact from HMI with id: " << id.toInt() ;
    mInstance.deleteContact(id.toInt());
}


