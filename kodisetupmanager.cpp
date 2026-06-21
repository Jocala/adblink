#include "kodisetupmanager.h"
#include "devicerecord.h"
#include "getadbdata.h"
#include "getreturncode.h"
#include "adbutils.h"
#include "logfile.h"

#include <QMessageBox>
#include <QRegularExpression>
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
    Q_UNUSED(runLongProcess);

    QString cstring;
    QString command;

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

    QString mcpath = QStringLiteral("/sdcard/kodi_data/") + device.xbmcpackage;
    QString expectedEnv = QStringLiteral("xbmc.data=") + mcpath + QStringLiteral("/files");
    QString kodiDir = mcpath + QStringLiteral("/files/.kodi");

    // Read current state
    cstring = adbPrefix + " shell ls " + kodiDir;
    QString dirResult = getadbOutput(cstring);
    bool dirExists = !dirResult.contains("No such file or directory");

    cstring = adbPrefix + " shell cat /sdcard/xbmc_env.properties";
    QString envContent = getadbOutput(cstring);
    envContent.replace(QRegularExpression("[\r\n]"), "");
    bool envCorrect = !envContent.isEmpty() && envContent == expectedEnv;

    // Determine what needs fixing
    bool needDir = !dirExists;
    bool needEnv = !envCorrect;

    if (!needDir && !needEnv) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Kodi data area is already set up correctly on %1").arg(device.daddr));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        logfile(device.daddr + ": Kodi data area already OK");
        return;
    }

    if (needDir) {
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

        cstring = adbPrefix + " shell mkdir -p " + kodiDir;
        command = getadbOutput(cstring);
        logfile(command);

        cstring = adbPrefix + " shell ls " + kodiDir;
        command = getadbOutput(cstring);
        if (command.contains("No such file or directory")) {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Error creating Kodi data folder"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            logfile("Kodi data folder creation failed");
            return;
        }
    }

    if (needEnv) {
        cstring = adbPrefix + " shell echo " + expectedEnv + " > /sdcard/xbmc_env.properties";
        command = getadbOutput(cstring);
        logfile("Updated /sdcard/xbmc_env.properties");
        logfile(command);
    }

    QStringList parts;
    if (needDir) parts << QStringLiteral("directory created");
    if (needEnv) parts << QStringLiteral("env file written");
    QString detail = parts.join(QStringLiteral(", "));

    QMessageBox msgBox(parentWidget);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(QString());
    msgBox.setText(QStringLiteral("Kodi data area created: %1").arg(detail));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.exec();
}
