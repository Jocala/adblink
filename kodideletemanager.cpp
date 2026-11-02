#include "kodideletemanager.h"
#include "devicerecord.h"
#include "adbutils.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QStringList>
#include <QWidget>

static bool isKodiRunning(const QString &adbPrefix, const QString &package)
{
    QString output = getadbOutput(adbPrefix + " shell pidof " + package);
    if (output.contains("not found"))
        return getadbOutput(adbPrefix + " shell ps").contains(package);

    const QStringList pids = output.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    for (const QString &pid : pids) {
        if (pid.toInt() > 0)
            return true;
    }
    return false;
}

KodiDeleteManager::KodiDeleteManager(QObject *parent)
    : QObject(parent)
{
}

void KodiDeleteManager::deleteKodiData(QWidget *parentWidget,
                                       const DeviceRecord &device,
                                       const QString &adbPrefix,
                                       RunLongProcessCallback runLongProcess)
{
    QString cstring;
    QString command;

    if (isKodiRunning(adbPrefix, device.xbmcpackage)) {
        QMessageBox msgBox(parentWidget);
        msgBox.setWindowTitle(QStringLiteral("Stop Kodi"));
        msgBox.setText(QStringLiteral("Cannot delete Kodi data while Kodi is running.\nStop %1 on device?").arg(device.xbmcpackage));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowModality(Qt::WindowModal);
        if (msgBox.exec() != QMessageBox::Yes) {
            logfile(device.daddr + ": delete Kodi data cancelled, " + device.xbmcpackage + " running");
            return;
        }

        cstring = adbPrefix + " shell am force-stop " + device.xbmcpackage;
        command = getadbOutput(cstring);
        if (!command.trimmed().isEmpty())
            logfile(device.daddr + ": force-stop " + device.xbmcpackage + ": " + command.trimmed());
    }

    const QString kodiPath = resolveKodiPath(adbPrefix, QStringLiteral("/sdcard/"),
                                             device.xbmcpackage, isScopedStorage(adbPrefix));

    cstring = adbPrefix + " shell ls " + kodiPath;
    command = getadbOutput(cstring);
    if (command.contains("No such file or directory")) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Kodi data not found!\n\n%1").arg(kodiPath));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        logfile(device.daddr + ": Kodi data not found: " + kodiPath);
        return;
    }

    QMessageBox msgBox(parentWidget);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(QString());
    msgBox.setText(QStringLiteral("Delete Kodi data?\n\n%1\n\n"
                                  "Settings, addons and library data will be removed.\n"
                                  "Kodi will start with a fresh setup.")
                       .arg(kodiPath));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setWindowModality(Qt::WindowModal);
    if (msgBox.exec() != QMessageBox::Yes) {
        logfile(device.daddr + ": delete Kodi data cancelled");
        return;
    }

    logfile(device.daddr + ": deleting Kodi data: " + kodiPath);
    cstring = adbPrefix + " shell rm -r " + kodiPath;
    command = runLongProcess(cstring, "Deleting Kodi data for " + device.daddr);
    if (!command.trimmed().isEmpty())
        logfile(device.daddr + ": delete output: " + command.trimmed());

    cstring = adbPrefix + " shell ls " + kodiPath;
    command = getadbOutput(cstring);
    const bool removed = command.contains("No such file or directory");

    QMessageBox resultBox(parentWidget);
    resultBox.setWindowTitle(QString());
    resultBox.setStandardButtons(QMessageBox::Ok);
    resultBox.setWindowModality(Qt::WindowModal);
    if (removed) {
        resultBox.setIcon(QMessageBox::Information);
        resultBox.setText(QStringLiteral("Kodi data deleted.\n\n%1").arg(kodiPath));
        logfile(device.daddr + ": Kodi data deleted: " + kodiPath);
    } else {
        resultBox.setIcon(QMessageBox::Critical);
        resultBox.setText(QStringLiteral("Kodi data was not deleted. See adblink.log."));
        logfile(device.daddr + ": Kodi data delete failed: " + kodiPath);
    }
    resultBox.exec();
}
