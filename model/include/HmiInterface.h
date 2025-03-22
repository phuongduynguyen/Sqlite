#ifndef HMI_INTERFACE
#define HMI_INTERFACE
#include <QObject>
#include <QDebug>
#include "DataEngine.h"
class HmiInterface : public QObject
{
    Q_OBJECT
    public:
        explicit HmiInterface(QObject *parent = nullptr) : QObject(parent), mInstance(DataEngine::getInstance())
        {

        }
        Q_INVOKABLE void sendMessage(const QString &msg);
        Q_INVOKABLE void addContactInterface(const QString& name, const std::vector<QString>& numbers, const QString& notes, const QString& uri);
        Q_INVOKABLE void deleteContactInterface(const QString& id);
    private:
        DataEngine& mInstance;
};

#endif