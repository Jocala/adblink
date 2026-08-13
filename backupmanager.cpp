#include "backupmanager.h"
#include "adbdevice.h"
#include "adbutils.h"
#include "kodidatamanager.h"
#include "restdialog.h"
#include "restoreoptionsdialog.h"
#include "getadbdata.h"
#include "getreturncode.h"
#include "logfile.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
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
        const QString envPrefix("xbmc.data=");
        int idx = envContent.indexOf(envPrefix);
        if (idx >= 0) {
            QString envPath = envContent.mid(idx + envPrefix.length()).trimmed();
            if (envPath.startsWith("/"))
                return envPath + QStringLiteral("/.kodi");
        }
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
    ++m_activeBackups;
    logfile("Starting backup for " + device.daddr);

    bool diag = false;
    {
        QFile f(jsonstring);
        if (f.open(QIODevice::ReadOnly)) {
            diag = QJsonDocument::fromJson(f.readAll()).object().value("diagnostic").toBool();
            f.close();
        }
    }
    if (diag)
        ::logDeviceDiagnostics(getadbpath() + " -s " + device.daddr, device.daddr,
                               device.xbmcpackage, device.data_root);

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
            --m_activeBackups;
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

    if (diag) {
        logfile(device.daddr + ": backup n_data_root=" + n_data_root
                + " scoped=" + (scoped ? "true" : "false")
                + " mcpath=" + mcpath);
    }

    cstring = adbPrefix + "shell ls " + mcpath + "/files/.kodi";
    if (!getreturncode(cstring)) {
        QMessageBox msgBox(parent);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Kodi's files not found at %1").arg(mcpath));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        logfile(device.daddr + ": Error: Kodi's files not found at " + mcpath);
        --m_activeBackups;
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
        --m_activeBackups;
        return false;
    }

    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(QStringLiteral("Backup"));
    msgBox.setText(QStringLiteral("Backup to %1 for %2?").arg(dir, device.daddr));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setWindowModality(Qt::WindowModal);
    if (msgBox.exec() != QMessageBox::Yes) {
        --m_activeBackups;
        return false;
    }

    mcpath = mcpath + "/";
    dir = dir + "/";
    if (osType == 1)
        dir.replace("/", "\\");

    cstring = adbPrefix + "pull " + mcpath + "files/.kodi/. " + '"' + dir + '"';
    if (diag)
        logfile(device.daddr + ": backup command: " + cstring);
    QString command = runLongProcess(cstring, "backup running for " + device.daddr);

    removeMetadataFiles(dir);

    if (QDir(dir + "userdata").exists()) {
        if (dataManager && !jsonstring.isEmpty())
            dataManager->writeBackupPath(jsonstring, dir);
        logfile("backup completed successfully for " + device.daddr);
        logfile("backup location: " + dir);
        if (--m_activeBackups == 0) {
            QMessageBox msgBox(parent);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Backup complete. See log."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
        }
        return true;
    } else {
        QMessageBox msgBox(parent);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Backup failed for %1. See log.").arg(device.daddr));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        logfile(device.daddr + ": Error: Backup failed: " + command);
        --m_activeBackups;
        return false;
    }
}

bool BackupManager::restoreDevice(QWidget *parent, const DeviceRecord &device,
                                   const QString &adbPrefix, bool scoped,
                                   const QString &jsonstring,
                                   KodiDataManager *dataManager,
                                   std::function<QString(const QString&, const QString&)> runLongProcess)
{
    ++m_activeRestores;
    logfile("Starting restore for " + device.daddr);

    bool diag = false;
    {
        QFile f(jsonstring);
        if (f.open(QIODevice::ReadOnly)) {
            diag = QJsonDocument::fromJson(f.readAll()).object().value("diagnostic").toBool();
            f.close();
        }
    }
    if (diag)
        ::logDeviceDiagnostics(getadbpath() + " -s " + device.daddr, device.daddr,
                               device.xbmcpackage, device.data_root);

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
        mcpath.clear();
        const QString envPrefix("xbmc.data=");
        int idx = command.indexOf(envPrefix);
        if (idx >= 0) {
            mcpath = command.mid(idx + envPrefix.length()).trimmed();
            bool valid = mcpath.startsWith("/");
            if (valid && mcpath.endsWith("/files"))
                mcpath.chop(6);
            else
                valid = false;
            if (valid) {
                QString suffix = QStringLiteral("/") + device.xbmcpackage;
                if (mcpath.endsWith(suffix)) {
                    QString structure = mcpath.left(mcpath.length() - suffix.length());
                    bool isScopedStruc = structure.endsWith(QStringLiteral("/kodi_data"));
                    bool isLegacyStruc = structure.endsWith(QStringLiteral("/Android/data"));
                    if (mcpath.startsWith("/sdcard/")) {
                        if ((scoped && !isScopedStruc) || (!scoped && !isLegacyStruc))
                            valid = false;
                    } else if (!isLegacyStruc) {
                        valid = false;
                    }
                } else {
                    valid = false;
                }
            }
            if (!valid)
                mcpath.clear();
        }
        if (!mcpath.isEmpty())
            xbmc_env = true;
        if (diag) {
            logfile(device.daddr + ": xbmc_env.properties content: " + command);
            logfile(device.daddr + ": env parsed mcpath: \"" + mcpath + "\"");
        }
    }

    cstring = getadbpath() + " -s " + device.daddr + " shell ps | grep " + device.xbmcpackage;
    command = getadbOutput(cstring);

    if (command.contains(device.xbmcpackage)) {
        QMessageBox msgBox(parent);
        msgBox.setWindowTitle(QStringLiteral("Stop Kodi"));
        msgBox.setText(QStringLiteral("Cannot restore while Kodi is running on %1.\n Stop %2?").arg(device.daddr, device.xbmcpackage));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowModality(Qt::WindowModal);
        QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());
        if (reply == QMessageBox::Yes) {
            cstring = getadbpath() + " -s " + device.daddr + " shell am force-stop " + device.xbmcpackage;
            getadbOutput(cstring);
        } else {
            logfile(device.daddr + ": Error: " + device.xbmcpackage + " running. Restore failed");
            --m_activeRestores;
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
                --m_activeRestores;
                return false;
            }
        }

        if (n_data_root.isEmpty())
            n_data_root = "/sdcard";

        if (!n_data_root.startsWith("/"))
            n_data_root.prepend("/");
        if (!n_data_root.endsWith("/"))
            n_data_root.append("/");

        if (diag) {
            logfile(device.daddr + ": restore n_data_root=" + n_data_root
                    + " scoped=" + (scoped ? "true" : "false"));
        }

        if (scoped) {
            kbase = n_data_root + "kodi_data/";
            mcpath = kbase + device.xbmcpackage;

            cstring = getadbpath() + " -s " + device.daddr + " shell mkdir -p " + kbase;
            command = getadbOutput(cstring);
            if (command.contains("No such file or directory")) {
                QMessageBox msgBox(parent);
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setWindowTitle(QString());
                msgBox.setText(QStringLiteral("Failed to create kodi_data directory on %1").arg(device.daddr));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setWindowModality(Qt::WindowModal);
                msgBox.exec();
                logfile(device.daddr + ": Error creating kodi_data: " + command);
                --m_activeRestores;
                return false;
            }
        } else {
            mcpath = n_data_root + "Android/data/" + device.xbmcpackage;
            kbase = n_data_root + "Android/data/";
        }
        if (diag)
            logfile(device.daddr + ": restore mcpath=" + mcpath);
    }

    QString backup;
    if (dataManager && !jsonstring.isEmpty())
        backup = dataManager->readBackupPath(jsonstring);
    QDir backupDir(backup);
    QString dir = QFileDialog::getExistingDirectory(parent, "Choose Backup Folder for " + device.daddr,
                                                     backupDir.absolutePath(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        --m_activeRestores;
        return false;
    }

    if (!QDir(dir + "/userdata").exists()) {
        QMessageBox msgBox(parent);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Invalid backup for %1. No userdata folder.").arg(device.daddr));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        logfile(device.daddr + ": Error: Invalid backup. No userdata folder.");
        --m_activeRestores;
        return false;
    }
    if (!QDir(dir + "/addons").exists()) {
        QMessageBox msgBox(parent);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Invalid backup for %1. addons folder not found.").arg(device.daddr));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        logfile(device.daddr + ": Error: Invalid backup. addons folder not found.");
        --m_activeRestores;
        return false;
    }

    QStringList protectFiles;
    if (dataManager && !jsonstring.isEmpty())
        protectFiles = dataManager->readProtectFiles(jsonstring);

    RestoreOptionsDialog restoreDialog(device.daddr, protectFiles, parent);
    restoreDialog.setWindowModality(Qt::WindowModal);
    restoreDialog.setModal(true);
    if (restoreDialog.exec() != QDialog::Accepted) {
        --m_activeRestores;
        return false;
    }
    protectFiles = restoreDialog.checkedFiles();
    if (dataManager && !jsonstring.isEmpty())
        dataManager->writeProtectFiles(jsonstring, protectFiles);

    QString protectDir;
    if (!protectFiles.isEmpty()) {
        protectDir = QDir::tempPath() + QStringLiteral("/adblink-protect-") + device.daddr;
        protectDir.replace(':', '_');
        QDir().mkpath(protectDir);
        QString userdataPath = mcpath + QStringLiteral("/files/.kodi/userdata/");
        for (int i = 0; i < protectFiles.size(); i++) {
            cstring = adbPrefix + "shell ls " + userdataPath + protectFiles.at(i);
            if (getreturncode(cstring)) {
                cstring = adbPrefix + "pull " + userdataPath + protectFiles.at(i)
                          + " \"" + protectDir + "/" + protectFiles.at(i) + "\"";
                if (diag)
                    logfile(device.daddr + ": restore protect pull: " + cstring);
                runLongProcess(cstring, "protecting " + protectFiles.at(i) + " for " + device.daddr);
            } else {
                logfile(device.daddr + ": protect file not on device, skipping: " + protectFiles.at(i));
            }
        }
    }

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
            QMessageBox msgBox(parent);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Error creating restore point on %1").arg(device.daddr));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            logfile(device.daddr + ": Error creating restore point: " + errorOutput);
            if (!protectDir.isEmpty())
                QDir(protectDir).removeRecursively();
            --m_activeRestores;
            return false;
        }
    }

    removeMetadataFiles(dir);

    dir = dir + "/.";

    cstring = adbPrefix + "push \"" + dir + "\" " + mcpath + "/files/.kodi/";
    if (diag)
        logfile(device.daddr + ": restore command: " + cstring);
    command = runLongProcess(cstring, "restore running for " + device.daddr);

    if (command.contains("bytes")) {
        if (!protectDir.isEmpty()) {
            QString userdataPath = mcpath + QStringLiteral("/files/.kodi/userdata/");
            QDir pd(protectDir);
            QStringList pulled = pd.entryList(QDir::Files, QDir::Name);
            for (int i = 0; i < pulled.size(); i++) {
                cstring = adbPrefix + "push \"" + protectDir + "/" + pulled.at(i)
                          + "\" " + userdataPath + pulled.at(i);
                if (diag)
                    logfile(device.daddr + ": restore protect push: " + cstring);
                command = getadbOutput(cstring);
                if (!command.contains("1 file pushed"))
                    logfile(device.daddr + ": Error restoring protected file " + pulled.at(i) + ": " + command);
            }
            QDir(protectDir).removeRecursively();
        }
        cstring = getadbpath() + " -s " + device.daddr + " shell rm /sdcard/xbmc_env.properties";
        getadbOutput(cstring);

        if (scoped) {
            cstring = getadbpath() + " -s " + device.daddr + " shell echo xbmc.data=" + mcpath + "/files > /sdcard/xbmc_env.properties";
            command = getadbOutput(cstring);
            if (command.contains("No such file or directory") || !command.isEmpty()) {
                QMessageBox msgBox(parent);
                msgBox.setIcon(QMessageBox::Critical);
                msgBox.setWindowTitle(QString());
                msgBox.setText(QStringLiteral("Failed to create xbmc_env.properties on %1").arg(device.daddr));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setWindowModality(Qt::WindowModal);
                msgBox.exec();
                logfile(device.daddr + ": Error creating xbmc_env.properties: " + command);
                --m_activeRestores;
                return false;
            }
        } else if (n_data_root != "/sdcard/") {
            cstring = getadbpath() + " -s " + device.daddr + " shell echo xbmc.data=" + mcpath + "/files > /sdcard/xbmc_env.properties";
            getadbOutput(cstring);
        }

        if (dataManager && !jsonstring.isEmpty())
            dataManager->writeBackupPath(jsonstring, dir);
        logfile("Restore completed successfully for " + device.daddr);
        if (--m_activeRestores == 0) {
            QMessageBox msgBox(parent);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Restore complete. See log."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
        }
        return true;
    } else {
        QMessageBox msgBox(parent);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Restore failed for %1. See log.").arg(device.daddr));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        logfile(device.daddr + ": Error: Restore failed: " + command);
        if (!protectDir.isEmpty())
            QDir(protectDir).removeRecursively();
        --m_activeRestores;
        return false;
    }
}
