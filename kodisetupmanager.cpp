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
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(parentWidget, "Stop Kodi",
            "Cannot create path while Kodi is running.\n Stop " + device.xbmcpackage + " on device?",
            QMessageBox::Yes | QMessageBox::No);
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
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(parentWidget, "Create Kodi Data",
            "This will overwrite /sdcard/kodi_data/\nProceed?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
            return;
    }

    {
        cstring = adbPrefix + " shell getprop ro.build.version.release";
        QString android = getadbOutput(cstring);
        if (android.toInt() >= 11) {
            if (isPackageInstalled(adbPrefix, device.xbmcpackage)) {
                cstring = adbPrefix + " shell appops set --uid " + device.xbmcpackage + " MANAGE_EXTERNAL_STORAGE allow";
                if (!getreturncode(cstring))
                    QMessageBox::critical(parentWidget, "", "Error setting Kodi permissions");
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
            QMessageBox::critical(parentWidget, "", "Error creating Kodi data folder");
            logfile("Restore error:" + errorp);
            return;
        }
    }

    cstring = adbPrefix + " shell echo xbmc.data=" + mcpath + "/files > /sdcard/xbmc_env.properties";
    command = getadbOutput(cstring);
    logfile("create /sdcard/xbmc_env.properties");
    logfile(command);

    QMessageBox::information(parentWidget, "", "Kodi data area created");
}
