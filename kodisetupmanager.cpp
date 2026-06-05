#include "kodisetupmanager.h"
#include "devicerecord.h"
#include "getadbdata.h"
#include "getreturncode.h"
#include "adbutils.h"
#include "logfile.h"

#include <QMessageBox>
#include <QWidget>

KodiSetupManager::KodiSetupManager(QObject *parent)
    : QObject(parent)
{
}

void KodiSetupManager::createKodiData(QWidget *parentWidget,
                                       const DeviceRecord &device,
                                       const QString &adbPrefix,
                                       RunLongProcessCallback runLongProcess)
{
    QString cstring;
    QString command;
    QString mcpath;

    mcpath = "/sdcard/kodi_data/" + device.xbmcpackage;

    cstring = adbPrefix + " shell ps | grep " + device.xbmcpackage;
    command = getadbOutput(cstring);

    if (command.contains(device.xbmcpackage)) {
        QMessageBox msgBox(parentWidget);
        msgBox.setWindowTitle(QStringLiteral("Stop Kodi"));
        msgBox.setText(QStringLiteral("Cannot create path while Kodi is running.\n Stop %1 on device?").arg(device.xbmcpackage));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowModality(Qt::WindowModal);
        QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());
        if (reply == QMessageBox::Yes) {
            cstring = adbPrefix + " shell am force-stop " + device.xbmcpackage;
            command = getadbOutput(cstring);
            logfile(command);
        } else {
            logfile(device.xbmcpackage + " running. Path creation failed");
            return;
        }
    }

    cstring = adbPrefix + " shell ls " + mcpath;
    command = getadbOutput(cstring);

    if (!command.contains("No such file or directory")) {
        QMessageBox msgBox(parentWidget);
        msgBox.setWindowTitle(QStringLiteral("Create Kodi Data"));
        msgBox.setText(QStringLiteral("This will overwrite /sdcard/kodi_data/\nProceed?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowModality(Qt::WindowModal);
        QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());
        if (reply == QMessageBox::No)
            return;
    }

    {
        cstring = adbPrefix + " shell getprop ro.build.version.release";
        QString android = getadbOutput(cstring);
        if (android.toInt() >= 11) {
            if (isPackageInstalled(adbPrefix, device.xbmcpackage)) {
                cstring = adbPrefix + " shell appops set --uid " + device.xbmcpackage + " MANAGE_EXTERNAL_STORAGE allow";
                if (!getreturncode(cstring)) {
                    QMessageBox msgBox(parentWidget);
                    msgBox.setIcon(QMessageBox::Critical);
                    msgBox.setWindowTitle(QString());
                    msgBox.setText(QStringLiteral("Error setting Kodi permissions"));
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.setWindowModality(Qt::WindowModal);
                    msgBox.exec();
                }
            }
        }
    }

    cstring = adbPrefix + " shell rm -r " + mcpath;
    command = runLongProcess(cstring, "Preparing target");
    logfile(command);

    cstring = adbPrefix + " shell ls " + mcpath;
    command = getadbOutput(cstring);

    if (command.contains("No such file or directory")) {
        cstring = adbPrefix + " shell mkdir -p " + mcpath + "/files/.kodi";
        command = getadbOutput(cstring);
        logfile(command);
        QString errorp = command;
        cstring = adbPrefix + " shell ls " + mcpath + "/files/.kodi";
        command = getadbOutput(cstring);

        if (command.contains("No such file or directory")) {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Error creating Kodi data folder"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            logfile("Restore error:" + errorp);
            return;
        }
    }

    cstring = adbPrefix + " shell echo xbmc.data=" + mcpath + "/files > /sdcard/xbmc_env.properties";
    command = getadbOutput(cstring);
    logfile("create /sdcard/xbmc_env.properties");
    logfile(command);

    QMessageBox msgBox(parentWidget);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(QString());
    msgBox.setText(QStringLiteral("Kodi data area created"));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.exec();
}
