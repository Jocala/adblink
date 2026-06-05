#include "datausagemanager.h"
#include "devicerecord.h"
#include "getadbdata.h"
#include "getreturncode.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QWidget>

DataUsageManager::DataUsageManager(QObject *parent)
    : QObject(parent)
{
}

void DataUsageManager::showKodiDataUsage(QWidget *parentWidget,
                                          const DeviceRecord &device,
                                          const QString &adbPrefix,
                                          RunLongProcessCallback runLongProcess)
{
    QString kodidata;
    QString cstring;
    QString command;
    QString xbmcpath;

    cstring = adbPrefix + " shell ls /sdcard/xbmc_env.properties";
    command = getadbOutput(cstring);

    if (!command.contains("No such file")) {
        cstring = adbPrefix + " shell cat /sdcard/xbmc_env.properties";
        command = getadbOutput(cstring);

        command.replace(QRegularExpression("[\r\n]"), "");

        int startIndex = command.indexOf("=") + 1;
        int endIndex = command.indexOf(".kodi") + 5;
        xbmcpath = command.mid(startIndex, endIndex - startIndex);
    } else {
        xbmcpath = "/sdcard/Android/data/" + device.xbmcpackage;
    }

    cstring = adbPrefix + " shell du -sh " + xbmcpath;
    command = runLongProcess(cstring, "calculating data size");

    if (command.contains("No such file")) {
        kodidata = "No data found";
    } else {
        int z = command.indexOf("G");
        if (z == -1)
            z = command.indexOf("M");
        if (z == -1)
            z = command.indexOf("K");

        if (z != -1)
            kodidata = command.mid(0, z + 1);
    }

    QMessageBox msgBox(parentWidget);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(QStringLiteral("Kodi Data"));
    msgBox.setText(QStringLiteral("Kodi data size:  %1").arg(kodidata));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.exec();
}
