#include "test.h"
#include <QDebug>

// Test(QObject *parent) : QObject(parent) {
//     // Any initialization code here
// }

void Test::showMessage(const QString &msg) {
    qDebug() << "showMessage " << msg;
}