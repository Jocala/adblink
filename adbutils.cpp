#include "adbutils.h"
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include "getlocaladb.h"
#include "logfile.h"
#include "getadbdata.h"

QString getadbpath()
{
    QString databasedir;
    QString adbPath;

#ifdef Q_OS_WIN
    databasedir = QDir::homePath() + "/AppData/Roaming/.jocala/";
#else
    databasedir = QDir::homePath() + "/.jocala/";
#endif

  adbPath = QCoreApplication::applicationDirPath() + "/adbfiles/adb";

#ifdef Q_OS_WIN
    adbPath += ".exe"; // Windows-specific
#endif
    adbPath = QDir::cleanPath(adbPath);


    if (!getlocaladb().isEmpty())
    { adbPath=getlocaladb();
    }


    if (!QFile::exists(adbPath))  {
        logfile("Error: adb binary missing at path: " + adbPath);
  }

    return adbPath;



}

QString readBatteryLevel(const QString &adbPrefix)
{
    QString cstring = adbPrefix + " shell dumpsys battery";
    QString output = getadbOutput(cstring);
    QString batteryLevel = "Unknown";
    QString batteryStatus = "";
    bool batteryPresent = false;

    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.contains("present", Qt::CaseInsensitive)) {
            QStringList parts = trimmedLine.split(':');
            if (parts.size() > 1 && parts[1].trimmed() == "true") {
                batteryPresent = true;
            }
        }
        if (trimmedLine.contains("level", Qt::CaseInsensitive)) {
            QStringList parts = trimmedLine.split(':');
            if (parts.size() > 1) {
                batteryLevel = parts[1].trimmed();
            }
        }
        if (trimmedLine.contains("status", Qt::CaseInsensitive)) {
            QStringList parts = trimmedLine.split(':');
            if (parts.size() > 1) {
                QString statusValue = parts[1].trimmed();
                if (statusValue == "1") {
                    batteryStatus = " (unknown)";
                } else if (statusValue == "2") {
                    batteryStatus = " (charging)";
                } else if (statusValue == "3") {
                    batteryStatus = " (discharging)";
                } else if (statusValue == "4") {
                    batteryStatus = " (not charging)";
                } else if (statusValue == "5") {
                    batteryStatus = " (full)";
                } else {
                    batteryStatus = " (status: " + statusValue + ")";
                }
            }
        }
    }

    return batteryPresent ? (batteryLevel + batteryStatus) : "Unknown";
}

void displayOff(const QString &adbPrefix)
{
    QString cstring = adbPrefix + " shell input keyevent 26 ";
    ::getadbOutput(cstring);
}

void busybox_permissions(const QString &adbPrefix)
{
    QString cstring = adbPrefix + " shell chmod 755 /data/local/tmp/adblink/busybox";
    ::getadbOutput(cstring);
}

QString resolveKodiPath(const QString &adbPrefix, const QString &dataRoot,
                         const QString &xbmcpackage, bool scoped)
{
    QString cstring = adbPrefix + " shell ls /sdcard/xbmc_env.properties";
    QString result = ::getadbOutput(cstring);

    if (!result.contains("No such file")) {
        cstring = adbPrefix + " shell cat /sdcard/xbmc_env.properties";
        result = ::getadbOutput(cstring);
        result.replace(QRegularExpression("[\r\n]"), "");
        return result.mid(result.indexOf("xbmc.data=") + 10) + "/.kodi";
    }

    if (scoped)
        return dataRoot + "kodi_data/" + xbmcpackage + "/files/.kodi";

    return dataRoot + "Android/data/" + xbmcpackage + "/files/.kodi";
}

bool ensureBusyboxInstalled(QWidget *parent, const QString &adbPrefix, const QString &msg)
{
    QString busybox = '"' + QCoreApplication::applicationDirPath() + "/adbfiles/busybox" + '"';
    QString cstring;
    QString command;

    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(QString());
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setWindowModality(Qt::WindowModal);
    if (msgBox.exec() == QMessageBox::No)
        return false;

    cstring = adbPrefix + " shell rm -r /data/local/tmp/adblink";
    ::getadbOutput(cstring);

    cstring = adbPrefix + " shell mkdir -p /data/local/tmp/adblink";
    ::getadbOutput(cstring);

    cstring = adbPrefix + " push " + busybox + " /data/local/tmp/adblink/";
    command = ::getadbOutput(cstring);

    if (!command.contains("bytes")) {
        logfile("busybox install failed ");
        logfile(command);
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("busybox install failed. See log."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::ApplicationModal);
        msgBox.exec();
        return false;
    }

    logfile(command);
    cstring = adbPrefix + " shell chmod 755 /data/local/tmp/adblink/busybox";
    ::getadbOutput(cstring);

    cstring = adbPrefix + " shell /data/local/tmp/adblink/busybox --install -s /data/local/tmp/adblink";
    ::getadbOutput(cstring);

    busybox_permissions(adbPrefix);
    QMessageBox infoBox(parent);
    infoBox.setIcon(QMessageBox::Information);
    infoBox.setWindowTitle(QString());
    infoBox.setText(QStringLiteral("Busybox installed."));
    infoBox.setStandardButtons(QMessageBox::Ok);
    infoBox.setWindowModality(Qt::WindowModal);
    infoBox.exec();
    return true;
}

bool isPackageInstalled(const QString &adbPrefix, const QString &package)
{
    QString cstring = adbPrefix + " shell pm list packages";
    QString result = getadbOutput(cstring);
    return result.contains(package);
}

void syncWaitForProcess(QProcess &process, int timeoutMs)
{
    QElapsedTimer timer;
    if (timeoutMs > 0)
        timer.start();

    while (process.state() != QProcess::NotRunning) {
        if (process.waitForFinished(100))
            break;
        QCoreApplication::processEvents();
        if (timeoutMs > 0 && timer.elapsed() >= timeoutMs) {
            process.kill();
            process.waitForFinished(1000);
            break;
        }
    }
}

static QString scopedAdbOutput(const QString &adbPrefix, const QString &adbCommand)
{
    QString fullCommand = adbPrefix + " " + adbCommand;
    QStringList args = QProcess::splitCommand(fullCommand);
    QString program = args.takeFirst();

    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(program, args);
    process.waitForStarted();
    syncWaitForProcess(process);
    return process.readAll().trimmed();
}

bool isScopedStorage(const QString &adbPrefix)
{
    QString apiOutput = scopedAdbOutput(adbPrefix, "shell getprop ro.build.version.sdk");
    bool ok;
    int apiLevel = apiOutput.toInt(&ok);
    if (!ok || apiOutput.isEmpty())
        return false;
    if (apiLevel < 29)
        return false;

    bool restrictedAccess = false;
    QString touchOutput = scopedAdbOutput(adbPrefix, "shell touch /sdcard/Android/data/org.xbmc.kodi/files/test.txt");
    if (touchOutput.isEmpty() && !restrictedAccess) {
        scopedAdbOutput(adbPrefix, "shell rm /sdcard/Android/data/org.xbmc.kodi/files/test.txt");
    } else {
        restrictedAccess = touchOutput.contains("Permission denied", Qt::CaseInsensitive);
    }

    if (!restrictedAccess) {
        touchOutput = scopedAdbOutput(adbPrefix, "shell touch /sdcard/DCIM/test.txt");
        if (touchOutput.isEmpty()) {
            scopedAdbOutput(adbPrefix, "shell rm /sdcard/DCIM/test.txt");
        } else {
            restrictedAccess = touchOutput.contains("Permission denied", Qt::CaseInsensitive);
        }
    }

    QString lsOutput = scopedAdbOutput(adbPrefix, "shell ls -ld /sdcard/");
    if (lsOutput.isEmpty()) {
        logfile("Issue: Failed to get /sdcard/ permissions");
    } else {
        bool permissiveFs = lsOutput.contains("rwxrwxrwx");
        if (permissiveFs) {
            logfile("Issue: Permissive /sdcard/ permissions, vendor may bypass scoped storage");
            restrictedAccess = false;
        }
    }

    return (apiLevel >= 30) || (apiLevel == 29 && restrictedAccess);
}

void logDeviceDiagnostics(const QString &adbPrefix, const QString &daddr,
                          const QString &xbmcpackage, const QString &dataRoot)
{
    logfile("--- Device diagnostics for " + daddr + " ---");

    QString packageCheck = adbPrefix + " shell pm list packages";
    QString pkgResult = ::getadbOutput(packageCheck);
    logfile("Kodi package (" + xbmcpackage + "): "
            + (pkgResult.contains(xbmcpackage) ? "installed" : "not installed"));

    QString envCheck = adbPrefix + " shell ls /sdcard/xbmc_env.properties";
    QString envResult = ::getadbOutput(envCheck);
    if (!envResult.contains("No such file")) {
        logfile("xbmc_env.properties: found");
        QString envCat = adbPrefix + " shell cat /sdcard/xbmc_env.properties";
        QString envContent = ::getadbOutput(envCat);
        envContent.replace(QRegularExpression("[\r\n]"), "");
        logfile("xbmc_env.properties content: " + envContent);

        const QString prefix("xbmc.data=");
        int idx = envContent.indexOf(prefix);
        if (idx >= 0) {
            QString envPath = envContent.mid(idx + prefix.length()).trimmed();
            logfile("xbmc_env.properties parsed path: " + envPath);
            int startIndex = envContent.indexOf("=") + 1;
            int endIndex = envContent.indexOf(".kodi") + 5;
            QString oldMcpath = envContent.mid(startIndex, endIndex - startIndex);
            logfile("xbmc_env.properties old-style mcpath: \"" + oldMcpath + "\"");
        }
    } else {
        logfile("xbmc_env.properties: not found");
    }

    QString apiOutput = scopedAdbOutput(adbPrefix, "shell getprop ro.build.version.sdk");
    bool ok;
    int apiLevel = apiOutput.toInt(&ok);
    if (!ok || apiOutput.isEmpty())
        logfile("Scoped: failed to read API level");
    else
        logfile("Scoped: API level = " + QString::number(apiLevel));

    bool restrictedAccess = false;
    QString touch1 = scopedAdbOutput(adbPrefix, "shell touch /sdcard/Android/data/" + xbmcpackage + "/files/test.txt");
    if (touch1.isEmpty()) {
        logfile("Scoped: touch /sdcard/Android/data/" + xbmcpackage + "/files/test.txt -> allowed");
        scopedAdbOutput(adbPrefix, "shell rm /sdcard/Android/data/" + xbmcpackage + "/files/test.txt");
    } else {
        restrictedAccess = touch1.contains("Permission denied", Qt::CaseInsensitive);
        logfile("Scoped: touch /sdcard/Android/data/... -> "
                + QString(restrictedAccess ? "denied" : "error: " + touch1.trimmed()));
    }

    if (!restrictedAccess) {
        QString touch2 = scopedAdbOutput(adbPrefix, "shell touch /sdcard/DCIM/test.txt");
        if (touch2.isEmpty()) {
            logfile("Scoped: touch /sdcard/DCIM/test.txt -> allowed");
            scopedAdbOutput(adbPrefix, "shell rm /sdcard/DCIM/test.txt");
        } else {
            restrictedAccess = touch2.contains("Permission denied", Qt::CaseInsensitive);
            logfile("Scoped: touch /sdcard/DCIM/test.txt -> "
                    + QString(restrictedAccess ? "denied" : "error: " + touch2.trimmed()));
        }
    }

    QString lsOutput = scopedAdbOutput(adbPrefix, "shell ls -ld /sdcard/");
    if (lsOutput.isEmpty()) {
        logfile("Scoped: /sdcard/ permissions: unknown");
    } else {
        bool permissiveFs = lsOutput.contains("rwxrwxrwx");
        logfile("Scoped: /sdcard/ permissions: " + lsOutput.trimmed()
                + (permissiveFs ? " (permissive)" : ""));
        if (permissiveFs)
            restrictedAccess = false;
    }

    bool scopedVerdict = (apiLevel >= 30) || (apiLevel == 29 && restrictedAccess);
    logfile("Scoped: verdict = " + QString(scopedVerdict ? "true" : "false")
            + " (api=" + QString::number(apiLevel)
            + ", restricted=" + (restrictedAccess ? "yes" : "no") + ")");

    QString runningCheck = adbPrefix + " shell ps | grep " + xbmcpackage;
    QString runningResult = ::getadbOutput(runningCheck);
    logfile("Kodi running: " + QString(runningResult.contains(xbmcpackage) ? "yes" : "no"));

    QString scopedPath = dataRoot + "kodi_data/" + xbmcpackage + "/files/.kodi";
    QString legacyPath = dataRoot + "Android/data/" + xbmcpackage + "/files/.kodi";
    logfile("Scoped path exists: " + scopedPath);
    {
        QString checkScoped = adbPrefix + " shell ls " + scopedPath;
        QString note = ::getadbOutput(checkScoped).contains("No such file") ? "not found" : "found";
        logfile(QStringLiteral("  -> ") + note);
    }
    logfile("Legacy path exists: " + legacyPath);
    {
        QString checkLegacy = adbPrefix + " shell ls " + legacyPath;
        QString note = ::getadbOutput(checkLegacy).contains("No such file") ? "not found" : "found";
        logfile(QStringLiteral("  -> ") + note);
    }

    logfile("--- End device diagnostics ---");
}

void removeMetadataFiles(const QString &dirPath)
{
    QDirIterator it(dirPath, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    QStringList entries;
    while (it.hasNext())
        entries.prepend(it.next());
    for (const QString &entry : entries) {
        QFileInfo fi(entry);
        QString name = fi.fileName();
        if (name == QStringLiteral(".DS_Store")
            || name == QStringLiteral(".localized")
            || name == QStringLiteral("__MACOSX")
            || name == QStringLiteral(".AppleDouble")
            || name.startsWith(QStringLiteral("._"))
            || name.endsWith(QChar('~'))
            || name.endsWith(QStringLiteral(".swp"))
            || name.endsWith(QStringLiteral(".swo"))
            || (name.startsWith(QChar('#')) && name.endsWith(QChar('#')))) {
            logfile(QStringLiteral("removeMetadataFiles: ") + entry);
            if (fi.isDir())
                QDir(entry).removeRecursively();
            else
                QFile::remove(entry);
        }
    }
}
