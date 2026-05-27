#include "thumbnailmanager.h"
#include "devicerecord.h"
#include "adbutils.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QMessageBox>
#include <QWidget>

ThumbnailManager::ThumbnailManager(QObject *parent)
    : QObject(parent)
{
}

void ThumbnailManager::deleteThumbnails(QWidget *parentWidget,
                                         const DeviceRecord &device,
                                         const QString &adbPrefix,
                                         RunLongProcessCallback runLongProcess)
{
    QString cstring;
    QString command;

    QString mcpath = resolveKodiPath(adbPrefix, "/sdcard/", device.xbmcpackage, false);
    QString thumb = mcpath + "/userdata/Thumbnails";
    QString textures = mcpath + "/userdata/Database/Textures*.db";

    cstring = adbPrefix + " shell ls " + thumb;
    command = getadbOutput(cstring);

    if (command.contains("No such file or directory")) {
        QMessageBox::critical(parentWidget, "", "Thumbnails not found!");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parentWidget, "", "Delete Thumbnails?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        logfile("Removing Thumbnails");
        cstring = adbPrefix + " shell rm -r " + thumb;
        command = runLongProcess(cstring, "Removing Thumbnails");
        if (command.length() > 0)
            logfile("Thumbnail directory issue: " + command);
        cstring = adbPrefix + " shell rm -r " + textures;
        command = getadbOutput(cstring);
        if (command.length() > 0)
            logfile("Textures database issue: " + command);
    }

    if (command.length() > 0)
        QMessageBox::critical(parentWidget, "", "Errors. See log");
    else
        QMessageBox::information(parentWidget, "", "Thumnails deleted");
}
