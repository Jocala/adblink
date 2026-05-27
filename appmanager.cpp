#include "appmanager.h"
#include "devicerecord.h"
#include "getadbdata.h"
#include "logfile.h"
#include "forcequitdialog.h"

#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QWidget>

AppManager::AppManager(QObject *parent)
    : QObject(parent)
{
}

void AppManager::stopApp(QWidget *parentWidget, const DeviceRecord & /*device*/,
                          const QString &adbPrefix, const QString &databaseDir)
{
    QString jsonPath = QDir(databaseDir).filePath("adblink.json");
    QString stopapp = "org.xbmc.kodi";
    QJsonObject jsonObj;

    QFile file(jsonPath);
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(parentWidget, "Error", "Cannot read adblink.json.");
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (doc.isObject()) {
            jsonObj = doc.object();
            if (jsonObj.contains("stopapp"))
                stopapp = jsonObj["stopapp"].toString();
        }
    }

    forcequitDialog dialog(false, stopapp, parentWidget);
    dialog.setWindowModality(Qt::WindowModal);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString package = dialog.packagename();
    if (package.isEmpty()) {
        QMessageBox::critical(parentWidget, "Error", "Invalid package name.");
        return;
    }

    QString cstring = adbPrefix + " shell am force-stop " + package;
    QString command = getadbOutput(cstring);
    if (command.contains("error", Qt::CaseInsensitive))
        QMessageBox::warning(parentWidget, "Warning", "Failed to stop application.");
    logfile(cstring);
    logfile(command);

    if (stopapp != package) {
        jsonObj["stopapp"] = package;
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(parentWidget, "Error", "Cannot save configuration.");
            return;
        }
        file.write(QJsonDocument(jsonObj).toJson());
        file.close();
    }
}

void AppManager::startApp(QWidget *parentWidget, const DeviceRecord & /*device*/,
                           const QString &adbPrefix, const QString &databaseDir)
{
    QString jsonPath = QDir(databaseDir).filePath("adblink.json");
    QString startapp = "org.xbmc.kodi/org.xbmc.kodi.Splash";
    QJsonObject jsonObj;

    QFile file(jsonPath);
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(parentWidget, "Error", "Cannot read adblink.json.");
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (doc.isObject()) {
            jsonObj = doc.object();
            if (jsonObj.contains("startapp"))
                startapp = jsonObj["startapp"].toString();
        }
    }

    forcequitDialog dialog(true, startapp, parentWidget);
    dialog.setWindowModality(Qt::WindowModal);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString package = dialog.packagename();
    if (package.isEmpty()) {
        QMessageBox::critical(parentWidget, "Error", "Invalid package name.");
        return;
    }

    QString cstring = adbPrefix + " shell am start -n " + package;
    QString command = getadbOutput(cstring);
    if (command.contains("error", Qt::CaseInsensitive))
        QMessageBox::warning(parentWidget, "Warning", "Failed to start application.");
    logfile(cstring);
    logfile(command);

    if (startapp != package) {
        jsonObj["startapp"] = package;
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(parentWidget, "Error", "Cannot save configuration.");
            return;
        }
        file.write(QJsonDocument(jsonObj).toJson());
        file.close();
    }
}
