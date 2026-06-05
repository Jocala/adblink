#include "screencapmanager.h"
#include "devicerecord.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QWidget>

ScreenCapManager::ScreenCapManager(QObject *parent)
    : QObject(parent)
{
}

void ScreenCapManager::captureScreenshot(QWidget *parentWidget,
                                          const DeviceRecord &device,
                                          const QString &adbPrefix,
                                          const QString &databaseDir)
{
    QJsonObject obj;
    QFile file(databaseDir + "adblink.json");
    (void)file.open(QIODevice::ReadOnly);
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    obj = doc.object();
    QString pulldir = obj["download"].toString();
    file.close();

    QDateTime dateTime = QDateTime::currentDateTime();
    QString dtstr = dateTime.toString("yyyyMMdd_HHmmss") + ".png";

    QString cstring = adbPrefix + " shell screencap -p /data/local/tmp/" + dtstr;
    QString command = getadbOutput(cstring);

    logfile(cstring);

    if (!command.isEmpty()) {
        logfile(command);
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Screenshot failed: %1").arg(command));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    if (!device.pulldir.isEmpty())
        pulldir = device.pulldir;

    cstring = adbPrefix + " pull /data/local/tmp/" + dtstr + " " + pulldir;
    command = getadbOutput(cstring);

    logfile(cstring);
    logfile(command);

    QString localFilePath = pulldir + "/" + dtstr;
    QFileInfo fileInfo(localFilePath);
    if (!fileInfo.exists()) {
        logfile("Error: Pulled file does not exist at " + localFilePath);
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Failed to pull screenshot: File not found at %1").arg(localFilePath));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    cstring = adbPrefix + " shell rm /data/local/tmp/" + dtstr;
    command = getadbOutput(cstring);

    logfile(cstring);
    logfile(command);

    QMessageBox msgBox(parentWidget);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(QString());
    msgBox.setText(QStringLiteral("Screenshot %1 copied to %2").arg(dtstr, pulldir));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.exec();
}
