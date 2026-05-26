#include "remotepushmanager.h"
#include "devicerecord.h"
#include "adbutils.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QWidget>

RemotePushManager::RemotePushManager(QObject *parent)
    : QObject(parent)
{
}

void RemotePushManager::pushRemoteXml(QWidget *parentWidget,
                                       const DeviceRecord &device,
                                       const QString &adbPrefix)
{
    QString cstring;
    QString command;
    QString mcpath;

    if (!isPackageInstalled(adbPrefix, device.xbmcpackage)) {
        QMessageBox::critical(parentWidget, "", device.xbmcpackage + " not installed");
        return;
    }

    mcpath = resolveKodiPath(adbPrefix, "/sdcard/", device.xbmcpackage, false);

    cstring = adbPrefix + " shell ls " + mcpath;
    command = getadbOutput(cstring);

    if (command.contains("No such file or directory")) {
        QMessageBox::critical(parentWidget, "", "Kodi data not found" + mcpath);
        return;
    }

    mcpath = mcpath + "/userdata/keymaps/";
    cstring = adbPrefix + " shell ls " + mcpath;
    command = getadbOutput(cstring);

    if (command.contains("No such file or directory")) {
        cstring = adbPrefix + " mkdir -p " + mcpath;
        command = getadbOutput(cstring);
    }

    QString fileName = QFileDialog::getOpenFileName(parentWidget,
        "Choose remote xml file", QDir::homePath(), tr("Files (*.xml)"));

    if (!fileName.isEmpty()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(parentWidget, "Push",
            fileName + " selected. Continue?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
            return;

        cstring = adbPrefix + " push " + '"' + fileName + '"' + " " + mcpath + "/keyboard.xml";
        command = getadbOutput(cstring);

        logfile("push remote:" + command);

        if (command.contains("bytes")) {
            QMessageBox::information(parentWidget, "", "Remote xml installed.");
        } else {
            QMessageBox::critical(parentWidget, "", "Remote xml installation failed.");
        }
    }
}
