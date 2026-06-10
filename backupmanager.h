#ifndef BACKUPMANAGER_H
#define BACKUPMANAGER_H

#include <QObject>
#include <QStringList>
#include <functional>

#include "devicerecord.h"

class AdbDevice;
class KodiDataManager;
class QWidget;

class BackupManager : public QObject
{
    Q_OBJECT

public:
    explicit BackupManager(QObject *parent = nullptr);

    void setDataManager(KodiDataManager *dm) { m_dataManager = dm; }

    QStringList discoverStorageMounts(AdbDevice *device) const;
    QString resolveKodiPath(AdbDevice *device, bool scoped, const QString &dataRoot, const QString &package) const;

    QString buildBackupPullCommand(AdbDevice *device, const QString &mcpath, const QString &destDir) const;
    QString buildRestorePushCommand(AdbDevice *device, const QString &srcDir, const QString &mcpath) const;

    bool validateBackupDir(const QString &dir) const;

    QString kodiDataRoot(const QString &dataRoot, bool scoped, const QString &package) const;
    QString kodiBaseRoot(const QString &dataRoot, bool scoped) const;

    bool backupDevice(QWidget *parent, const DeviceRecord &device,
                      const QString &adbPrefix, bool scoped,
                      int osType,
                      const QString &jsonstring,
                      KodiDataManager *dataManager,
                      std::function<QString(const QString&, const QString&)> runLongProcess);

    bool restoreDevice(QWidget *parent, const DeviceRecord &device,
                       const QString &adbPrefix, bool scoped,
                       const QString &jsonstring,
                       KodiDataManager *dataManager,
                       std::function<QString(const QString&, const QString&)> runLongProcess);

signals:
    void logMessage(const QString &msg) const;

private:
    KodiDataManager *m_dataManager = nullptr;
    int m_activeBackups = 0;
    int m_activeRestores = 0;
};

#endif // BACKUPMANAGER_H
