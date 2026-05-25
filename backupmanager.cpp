#include "backupmanager.h"
#include "adbdevice.h"
#include "kodidatamanager.h"

#include <QDir>
#include <QRegularExpression>

BackupManager::BackupManager(QObject *parent)
    : QObject(parent)
{
}

QStringList BackupManager::discoverStorageMounts(AdbDevice *device) const
{
    QString output = device->runShell(QStringLiteral("/data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1"));
    QStringList list = output.split(QRegularExpression(QStringLiteral("[\r\n]+")), Qt::SkipEmptyParts);

    for (int i = 0; i < list.size(); i++) {
        list[i].remove(QChar::CarriageReturn);
        list[i].remove(QChar::LineFeed);
        if (list[i] == QStringLiteral("Android") ||
            list[i] == QStringLiteral("Permission denied") ||
            list[i] == QStringLiteral("/storage/emulated") ||
            list[i] == QStringLiteral("/storage") ||
            list[i] == QStringLiteral("/storage/self") ||
            list[i].isEmpty()) {
            list.removeAt(i);
            i--;
        }
    }

    list.insert(0, QStringLiteral("/sdcard"));
    return list;
}

QString BackupManager::resolveKodiPath(AdbDevice *device, bool scoped, const QString &dataRoot, const QString &package) const
{
    QString checkCmd = QStringLiteral("ls /sdcard/xbmc_env.properties");
    QString result = device->runShell(checkCmd);

    if (!result.contains(QStringLiteral("No such file"))) {
        QString catCmd = QStringLiteral("cat /sdcard/xbmc_env.properties");
        QString envContent = device->runShell(catCmd);
        envContent.replace(QRegularExpression(QStringLiteral("[\r\n]")), QString());
        int startIndex = envContent.indexOf(QChar('=')) + 1;
        int endIndex = envContent.indexOf(QStringLiteral(".kodi")) + 5;
        if (startIndex > 0 && endIndex > startIndex)
            return envContent.mid(startIndex, endIndex - startIndex);
    }

    return kodiDataRoot(dataRoot, scoped, package) + QStringLiteral("/files/.kodi");
}

QString BackupManager::kodiDataRoot(const QString &dataRoot, bool scoped, const QString &package) const
{
    if (scoped)
        return dataRoot + QStringLiteral("kodi_data/") + package;
    return dataRoot + QStringLiteral("Android/data/") + package;
}

QString BackupManager::kodiBaseRoot(const QString &dataRoot, bool scoped) const
{
    if (scoped)
        return dataRoot + QStringLiteral("kodi_data/");
    return dataRoot + QStringLiteral("Android/data/");
}

QString BackupManager::buildBackupPullCommand(AdbDevice *device, const QString &mcpath, const QString &destDir) const
{
    return device->adbPrefix() + QStringLiteral(" pull ") + mcpath + QStringLiteral("files/.kodi/. \"") + destDir + QStringLiteral("\"");
}

QString BackupManager::buildRestorePushCommand(AdbDevice *device, const QString &srcDir, const QString &mcpath) const
{
    return device->adbPrefix() + QStringLiteral(" push \"") + srcDir + QStringLiteral("\" ") + mcpath + QStringLiteral("/files/.kodi/");
}

bool BackupManager::validateBackupDir(const QString &dir) const
{
    return QDir(dir + QStringLiteral("/userdata")).exists() &&
           QDir(dir + QStringLiteral("/addons")).exists();
}
