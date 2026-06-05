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
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Thumbnails not found!"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    QMessageBox msgBox(parentWidget);
    msgBox.setWindowTitle(QString());
    msgBox.setText(QStringLiteral("Delete Thumbnails?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setWindowModality(Qt::WindowModal);
    if (msgBox.exec() == QMessageBox::Yes) {
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

    if (command.length() > 0) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Errors. See log"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
    } else {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Thumnails deleted"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
    }
}
