#ifndef TEST_H
#define TEST_H
#include <QObject>

class Test : public QObject {
    Q_OBJECT
    public:
    explicit Test(QObject *parent = nullptr) {}

    Q_INVOKABLE void showMessage(const QString &msg);
};
#endif // BACKEND_H