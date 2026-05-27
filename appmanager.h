#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>
#include <QString>

struct DeviceRecord;

class AppManager : public QObject
{
    Q_OBJECT

public:
    explicit AppManager(QObject *parent = nullptr);

    void stopApp(QWidget *parentWidget, const DeviceRecord &device,
                 const QString &adbPrefix, const QString &databaseDir);

    void startApp(QWidget *parentWidget, const DeviceRecord &device,
                  const QString &adbPrefix, const QString &databaseDir);
};

#endif // APPMANAGER_H
