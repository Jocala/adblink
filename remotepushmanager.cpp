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
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("%1 not installed").arg(device.xbmcpackage));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    mcpath = resolveKodiPath(adbPrefix, "/sdcard/", device.xbmcpackage, false);

    cstring = adbPrefix + " shell ls " + mcpath;
    command = getadbOutput(cstring);

    if (command.contains("No such file or directory")) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Kodi data not found%1").arg(mcpath));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
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
        QMessageBox msgBox(parentWidget);
        msgBox.setWindowTitle(QStringLiteral("Push"));
        msgBox.setText(QStringLiteral("%1 selected. Continue?").arg(fileName));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowModality(Qt::WindowModal);
        QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());

        if (reply == QMessageBox::No)
            return;

        cstring = adbPrefix + " push " + '"' + fileName + '"' + " " + mcpath + "/keyboard.xml";
        command = getadbOutput(cstring);

        logfile("push remote:" + command);

        if (command.contains("bytes")) {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Remote xml installed."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
        } else {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Remote xml installation failed."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
        }
    }
}
