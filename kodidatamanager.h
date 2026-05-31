#ifndef KODIDATAMANAGER_H
#define KODIDATAMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QStringList>

#include "devicerecord.h"

class QSqlDatabase;

class KodiDataManager : public QObject
{
    Q_OBJECT

public:
    explicit KodiDataManager(QObject *parent = nullptr);

    bool initializeDatabase(const QString &dbPath);
    void createTables();
    DeviceRecord queryDeviceRecord(const QString &description) const;
    DeviceRecord queryDeviceByDaddr(const QString &daddr) const;
    bool descriptionExists(const QString &description) const;
    void deleteRecord(const QString &description);

    void createJsonConfig(const QString &configPath);
    QString readJsonValue(const QString &configPath, const QString &key) const;
    void writeJsonValue(const QString &configPath, const QString &key, const QString &value);

    QString readBackupPath(const QString &configPath) const;
    void writeBackupPath(const QString &configPath, const QString &dir);
    QString readInstallPath(const QString &configPath) const;
    void writeInstallPath(const QString &configPath, const QString &install);
    QString readDonationValue(const QString &configPath) const;

    QString dataBaseDir;
    QString logFileDir;
    QString scriptDir;
    int os = 2;

    int lfontsize = 22;
    int mfontsize = 20;
    int sfontsize = 18;

signals:
    void logMessage(const QString &message) const;
};

#endif // KODIDATAMANAGER_H
