#include "adbutils.h"
#include <QCoreApplication>
#include <QDir>
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

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parent, "", msg,
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
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
        QMessageBox::critical(nullptr, "", "busybox install failed. See log.");
        return false;
    }

    logfile(command);
    cstring = adbPrefix + " shell chmod 755 /data/local/tmp/adblink/busybox";
    ::getadbOutput(cstring);

    cstring = adbPrefix + " shell /data/local/tmp/adblink/busybox --install -s /data/local/tmp/adblink";
    ::getadbOutput(cstring);

    busybox_permissions(adbPrefix);
    QMessageBox::information(parent, "", "Busybox installed.");
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
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
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
