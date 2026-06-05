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
                          const QString &adbPrefix, const QString &databaseDir,
                          const QString &configBasename)
{
    QString jsonPath = QDir(databaseDir).filePath(configBasename);
    QString stopapp = "org.xbmc.kodi";
    QJsonObject jsonObj;

    QFile file(jsonPath);
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QStringLiteral("Error"));
            msgBox.setText(QStringLiteral("Cannot read %1.").arg(configBasename));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (doc.isObject()) {
            jsonObj = doc.object();
            if (jsonObj.contains("stopapp"))
                stopapp = jsonObj["stopapp"].toString();
        }
    } else {
        jsonObj["stopapp"] = stopapp;
    }

    forcequitDialog dialog(false, stopapp, parentWidget);
    dialog.setWindowModality(Qt::WindowModal);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString package = dialog.packagename();
    if (package.isEmpty()) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QStringLiteral("Error"));
        msgBox.setText(QStringLiteral("Invalid package name."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    QString cstring = adbPrefix + " shell am force-stop " + package;
    QString command = getadbOutput(cstring);
    if (command.contains("error", Qt::CaseInsensitive)) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(QStringLiteral("Warning"));
        msgBox.setText(QStringLiteral("Failed to stop application."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
    }
    logfile(cstring);
    logfile(command);

    if (stopapp != package) {
        jsonObj["stopapp"] = package;
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QStringLiteral("Error"));
            msgBox.setText(QStringLiteral("Cannot save configuration."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            return;
        }
        file.write(QJsonDocument(jsonObj).toJson());
        file.close();
    }
}

void AppManager::startApp(QWidget *parentWidget, const DeviceRecord & /*device*/,
                           const QString &adbPrefix, const QString &databaseDir,
                           const QString &configBasename)
{
    QString jsonPath = QDir(databaseDir).filePath(configBasename);
    QString startapp = "org.xbmc.kodi/org.xbmc.kodi.Splash";
    QJsonObject jsonObj;

    QFile file(jsonPath);
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QStringLiteral("Error"));
            msgBox.setText(QStringLiteral("Cannot read %1.").arg(configBasename));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (doc.isObject()) {
            jsonObj = doc.object();
            if (jsonObj.contains("startapp"))
                startapp = jsonObj["startapp"].toString();
        }
    } else {
        jsonObj["startapp"] = startapp;
    }

    forcequitDialog dialog(true, startapp, parentWidget);
    dialog.setWindowModality(Qt::WindowModal);
    if (dialog.exec() != QDialog::Accepted)
        return;

    QString package = dialog.packagename();
    if (package.isEmpty()) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QStringLiteral("Error"));
        msgBox.setText(QStringLiteral("Invalid package name."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    QString cstring = adbPrefix + " shell am start -n " + package;
    QString command = getadbOutput(cstring);
    if (command.contains("error", Qt::CaseInsensitive)) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle(QStringLiteral("Warning"));
        msgBox.setText(QStringLiteral("Failed to start application."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
    }
    logfile(cstring);
    logfile(command);

    if (startapp != package) {
        jsonObj["startapp"] = package;
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QStringLiteral("Error"));
            msgBox.setText(QStringLiteral("Cannot save configuration."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            return;
        }
        file.write(QJsonDocument(jsonObj).toJson());
        file.close();
    }
}
