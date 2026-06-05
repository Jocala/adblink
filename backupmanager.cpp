#include "backupmanager.h"
#include "adbdevice.h"
#include "adbutils.h"
#include "kodidatamanager.h"
#include "restdialog.h"
#include "getadbdata.h"
#include "getreturncode.h"
#include "logfile.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
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

bool BackupManager::backupDevice(QWidget *parent, const DeviceRecord &device,
                                  const QString &adbPrefix, bool scoped,
                                  int osType,
                                  const QString &jsonstring,
                                  KodiDataManager *dataManager,
                                  std::function<QString(const QString&, const QString&)> runLongProcess)
{
    logfile("Starting backup for " + device.daddr);

    QString cstring = getadbpath() + " -s " + device.daddr + " shell /data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1";
    QString s = getadbOutput(cstring);

    QStringList list = s.split('\n');
    for (int i = 0; i < list.size(); i++) {
        list[i].remove('\r');
        list[i].remove('\n');
        if (list[i] == "Android" ||
            list[i] == "Permission denied" ||
            list[i] == "/storage/emulated" ||
            list[i] == "/storage" ||
            list[i] == "/storage/self" ||
            list[i].isEmpty()) {
            list.removeAt(i);
            i--;
        }
    }

    QString n_data_root = "/sdcard";
    list.insert(0, "/sdcard");

    if (list.count() > 1) {
        restDialog dialog(parent);
        dialog.setWindowModality(Qt::WindowModal);
        dialog.setWindowTitle("Backup for " + device.daddr);
        dialog.setadb_restore(list);
        if (dialog.exec() == QDialog::Accepted) {
            n_data_root = dialog.restore_data_root();
        } else {
            return false;
        }
    }

    if (!n_data_root.startsWith("/"))
        n_data_root.prepend("/");
    if (!n_data_root.endsWith("/"))
        n_data_root.append("/");

    QString mcpath;
    if (scoped) {
        mcpath = n_data_root + "kodi_data/" + device.xbmcpackage;
    } else {
        mcpath = n_data_root + "Android/data/" + device.xbmcpackage;
    }

    cstring = adbPrefix + "shell ls " + mcpath + "/files/.kodi";
    if (!getreturncode(cstring)) {
        QMessageBox::critical(parent, "", "Kodi's files not found at " + mcpath);
        logfile(device.daddr + ": Error: Kodi's files not found at " + mcpath);
        return false;
    }

    QString backup;
    if (dataManager && !jsonstring.isEmpty())
        backup = dataManager->readBackupPath(jsonstring);
    QDir backupDir(backup);
    QString dir = QFileDialog::getExistingDirectory(parent, "Choose Backup Destination for " + device.daddr,
                                                     backupDir.absolutePath(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        logfile(device.daddr + ": Error: No backup destination selected");
        return false;
    }

    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(QStringLiteral("Backup"));
    msgBox.setText(QStringLiteral("Backup to %1 for %2?").arg(dir, device.daddr));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setWindowModality(Qt::WindowModal);
    if (msgBox.exec() != QMessageBox::Yes)
        return false;

    mcpath = mcpath + "/";
    dir = dir + "/";
    if (osType == 1)
        dir.replace("/", "\\");

    cstring = adbPrefix + "pull " + mcpath + "files/.kodi/. " + '"' + dir + '"';
    QString command = runLongProcess(cstring, "backup running for " + device.daddr);

    if (QDir(dir + "userdata").exists()) {
        if (dataManager && !jsonstring.isEmpty())
            dataManager->writeBackupPath(jsonstring, dir);
        logfile("backup completed successfully for " + device.daddr);
        logfile("backup location: " + dir);
        return true;
    } else {
        QMessageBox::critical(parent, "", "Backup failed for " + device.daddr + ". See log.");
        logfile(device.daddr + ": Error: Backup failed: " + command);
        return false;
    }
}

bool BackupManager::restoreDevice(QWidget *parent, const DeviceRecord &device,
                                   const QString &adbPrefix, bool scoped,
                                   const QString &jsonstring,
                                   KodiDataManager *dataManager,
                                   std::function<QString(const QString&, const QString&)> runLongProcess)
{
    logfile("Starting restore for " + device.daddr);

    QString cstring;
    QString command;
    QString n_data_root;
    QString mcpath;
    QString kbase;
    bool xbmc_env = false;

    cstring = getadbpath() + " -s " + device.daddr + " shell ls /sdcard/xbmc_env.properties";
    command = getadbOutput(cstring);
    if (!command.contains("No such file")) {
        cstring = getadbpath() + " -s " + device.daddr + " shell cat /sdcard/xbmc_env.properties";
        command = getadbOutput(cstring);
        command.replace(QRegularExpression("[\r\n]"), "");
        int startIndex = command.indexOf("=") + 1;
        int endIndex = command.indexOf(".kodi") + 5;
        mcpath = command.mid(startIndex, endIndex - startIndex);
        xbmc_env = true;
    }

    cstring = getadbpath() + " -s " + device.daddr + " shell ps | grep " + device.xbmcpackage;
    command = getadbOutput(cstring);

    if (command.contains(device.xbmcpackage)) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(parent, "Stop Kodi",
                                      "Cannot restore while Kodi is running on " + device.daddr +
                                      ".\n Stop " + device.xbmcpackage + "?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            cstring = getadbpath() + " -s " + device.daddr + " shell am force-stop " + device.xbmcpackage;
            getadbOutput(cstring);
        } else {
            logfile(device.daddr + ": Error: " + device.xbmcpackage + " running. Restore failed");
            return false;
        }
    }

    if (!xbmc_env) {
        cstring = getadbpath() + " -s " + device.daddr + " shell /data/local/tmp/adblink/busybox find /storage -type d -maxdepth 1";
        QString storageOutput = getadbOutput(cstring);

        QStringList storageList = storageOutput.split('\n');
        for (int i = 0; i < storageList.size(); i++) {
            storageList[i].remove('\r');
            storageList[i].remove('\n');
            if (storageList[i] == "Android" ||
                storageList[i] == "Permission denied" ||
                storageList[i] == "/storage/emulated" ||
                storageList[i] == "/storage" ||
                storageList[i] == "/storage/self" ||
                storageList[i].isEmpty()) {
                storageList.removeAt(i);
                i--;
            }
        }

        storageList.insert(0, "/sdcard");

        if (storageList.count() > 1) {
            restDialog dialog(parent);
            dialog.setWindowModality(Qt::WindowModal);
            dialog.setWindowTitle("Restore for " + device.daddr);
            dialog.setadb_restore(storageList);
            if (dialog.exec() == QDialog::Accepted) {
                n_data_root = dialog.restore_data_root();
            } else {
                return false;
            }
        }

        if (n_data_root.isEmpty())
            n_data_root = "/sdcard";

        if (!n_data_root.startsWith("/"))
            n_data_root.prepend("/");
        if (!n_data_root.endsWith("/"))
            n_data_root.append("/");

        if (scoped) {
            kbase = n_data_root + "kodi_data/";
            mcpath = kbase + device.xbmcpackage;

            cstring = getadbpath() + " -s " + device.daddr + " shell mkdir -p " + kbase;
            command = getadbOutput(cstring);
            if (command.contains("No such file or directory")) {
                QMessageBox::critical(parent, "", "Failed to create kodi_data directory on " + device.daddr);
                logfile(device.daddr + ": Error creating kodi_data: " + command);
                return false;
            }
        } else {
            mcpath = n_data_root + "Android/data/" + device.xbmcpackage;
            kbase = n_data_root + "Android/data/";
        }
    }

    QString backup;
    if (dataManager && !jsonstring.isEmpty())
        backup = dataManager->readBackupPath(jsonstring);
    QDir backupDir(backup);
    QString dir = QFileDialog::getExistingDirectory(parent, "Choose Backup Folder for " + device.daddr,
                                                     backupDir.absolutePath(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return false;

    if (!QDir(dir + "/userdata").exists()) {
        QMessageBox::critical(parent, "", "Invalid backup for " + device.daddr + ". No userdata folder.");
        logfile(device.daddr + ": Error: Invalid backup. No userdata folder.");
        return false;
    }
    if (!QDir(dir + "/addons").exists()) {
        QMessageBox::critical(parent, "", "Invalid backup for " + device.daddr + ". addons folder not found.");
        logfile(device.daddr + ": Error: Invalid backup. addons folder not found.");
        return false;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parent, "Restore",
                                  "Restore this backup to " + device.daddr +
                                  "? This will overwrite existing Kodi data.",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
        return false;

    cstring = adbPrefix + "shell rm -r " + mcpath;
    runLongProcess(cstring, "preparing target for " + device.daddr);

    cstring = getadbpath() + " -s " + device.daddr + " shell ls " + mcpath;
    command = getadbOutput(cstring);

    if (command.contains("No such file or directory")) {
        cstring = getadbpath() + " -s " + device.daddr + " shell mkdir -p " + mcpath + "/files/.kodi";
        command = getadbOutput(cstring);
        QString errorOutput = command;

        cstring = getadbpath() + " -s " + device.daddr + " shell ls " + mcpath + "/files/.kodi";
        command = getadbOutput(cstring);

        if (command.contains("No such file or directory")) {
            QMessageBox::critical(parent, "", "Error creating restore point on " + device.daddr);
            logfile(device.daddr + ": Error creating restore point: " + errorOutput);
            return false;
        }
    }

    dir = dir + "/.";

    cstring = adbPrefix + "push \"" + dir + "\" " + mcpath + "/files/.kodi/";
    command = runLongProcess(cstring, "restore running for " + device.daddr);

    if (command.contains("bytes")) {
        cstring = getadbpath() + " -s " + device.daddr + " shell rm /sdcard/xbmc_env.properties";
        getadbOutput(cstring);

        if (scoped) {
            cstring = getadbpath() + " -s " + device.daddr + " shell echo xbmc.data=" + mcpath + "/files > /sdcard/xbmc_env.properties";
            command = getadbOutput(cstring);
            if (command.contains("No such file or directory") || !command.isEmpty()) {
                QMessageBox::critical(parent, "", "Failed to create xbmc_env.properties on " + device.daddr);
                logfile(device.daddr + ": Error creating xbmc_env.properties: " + command);
                return false;
            }
        } else if (n_data_root != "/sdcard/") {
            cstring = getadbpath() + " -s " + device.daddr + " shell echo xbmc.data=" + mcpath + "/files > /sdcard/xbmc_env.properties";
            getadbOutput(cstring);
        }

        if (dataManager && !jsonstring.isEmpty())
            dataManager->writeBackupPath(jsonstring, dir);
        logfile("Restore completed successfully for " + device.daddr);
        return true;
    } else {
        QMessageBox::critical(parent, "", "Restore failed for " + device.daddr + ". See log.");
        logfile(device.daddr + ": Error: Restore failed: " + command);
        return false;
    }
}
