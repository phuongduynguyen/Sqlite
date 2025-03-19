#ifndef TEST_H
#define TEST_H
#ifdef BUILD_QT
#include <QObject>

class Test : public QObject {
    Q_OBJECT
    public:
    explicit Test(QObject *parent = nullptr) {}

    Q_INVOKABLE void showMessage(const QString &msg);
};
#endif 
#endif // BACKEND_H