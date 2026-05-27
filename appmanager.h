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
                 const QString &adbPrefix, const QString &databaseDir,
                 const QString &configBasename = QStringLiteral("adblink.json"));

    void startApp(QWidget *parentWidget, const DeviceRecord &device,
                  const QString &adbPrefix, const QString &databaseDir,
                  const QString &configBasename = QStringLiteral("adblink.json"));
};

#endif // APPMANAGER_H
