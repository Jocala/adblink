#include "splashscreenmanager.h"
#include "adbutils.h"
#include "devicerecord.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>

void SplashScreenManager::installSplashScreen(QWidget *parent, const DeviceRecord &device,
                                               const QString &adbPrefix,
                                               RunLongProcessCallback runLongProcess)
{
    // Check busybox
    QString cstring = adbPrefix + " shell ls /data/local/tmp/adblink";
    QString command = getadbOutput(cstring);

    if (command.contains("No such file or directory"))
    {
        cstring = adbPrefix + " shell mkdir -p /data/local/tmp/adblink";
        getadbOutput(cstring);
    }

    cstring = adbPrefix + " shell ls /data/local/tmp/adblink/busybox";
    command = getadbOutput(cstring);

    if (command.contains("No such file or directory"))
    {
        QString busybox = '"' + QCoreApplication::applicationDirPath() + "/adbfiles/busybox" + '"';
        cstring = adbPrefix + " push " + busybox + " /data/local/tmp/adblink/";
        command = getadbOutput(cstring);

        if (!command.contains("bytes"))
        {
            logfile("busybox install failed ");
            logfile(command);
            QMessageBox::critical(nullptr, "", "busybox install failed. See log.");
            return;
        }

        logfile(command);

        cstring = adbPrefix + " shell chmod 755 /data/local/tmp/adblink/busybox";
        getadbOutput(cstring);

        cstring = adbPrefix + " shell /data/local/tmp/adblink/busybox --install -s /data/local/tmp/adblink";
        getadbOutput(cstring);
    }

    busybox_permissions(adbPrefix);

    // Check package
    cstring = adbPrefix + " shell pm list packages";
    command = getadbOutput(cstring);
    if (!command.contains(device.xbmcpackage))
    {
        QMessageBox::critical(parent, "", device.xbmcpackage + " not installed");
        return;
    }

    // Resolve Kodi path and ensure media directory exists
    QString mcpath = resolveKodiPath(adbPrefix, "/sdcard/", device.xbmcpackage, false);
    mcpath = mcpath + "/media";

    cstring = adbPrefix + " shell ls " + mcpath;
    command = getadbOutput(cstring);

    if (command.contains("No such file or directory"))
    {
        cstring = adbPrefix + " shell mkdir -p " + mcpath;
        getadbOutput(cstring);
    }

    // Pick splash file
    QString fileName = QFileDialog::getOpenFileName(parent, "Choose splash screen file",
                                                     QDir::homePath(),
                                                     tr("Files (*.png *.jpg *.jpeg)"));

    if (!fileName.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(parent, "Push", fileName + " selected. Continue?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            cstring = adbPrefix + " push " + '"' + fileName + '"' + " " + mcpath + "/splash.png";
            command = runLongProcess(cstring, "Splash Screen");
            logfile(command);

            if (command.contains("bytes"))
                QMessageBox::information(parent, "", "Splash screen installed.");
            else
                QMessageBox::critical(parent, "", "Splash screen installation failed.");
        }
    }
}

SplashScreenManager::SplashScreenManager(QObject *parent)
    : QObject(parent)
{
}
