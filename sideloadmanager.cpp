#include "sideloadmanager.h"
#include "devicerecord.h"
#include "logfile.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QWidget>

SideloadManager::SideloadManager(QObject *parent)
    : QObject(parent)
{
}

void SideloadManager::sideloadApks(QWidget *parentWidget,
                                    const DeviceRecord & /*device*/,
                                    ReadInstallCallback readInstall,
                                    InstallApkCallback installApk,
                                    WriteInstallCallback writeInstall)
{
    bool installer = false;

    QString install = readInstall();

    QStringList filenames = QFileDialog::getOpenFileNames(parentWidget,
        tr("APK files (*.apk);;All files (.*)"), install);

    if (!filenames.isEmpty()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(parentWidget, "Install", "Install APKs?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            logfile("starting APK installation(s)");
            for (int i = 0; i < filenames.count(); i++)
                installer = installApk(filenames.at(i));

            if (installer) {
                writeInstall(filenames[0].left(filenames[0].lastIndexOf('/')));
                QMessageBox::information(parentWidget, "",
                    "APK(s) installed.\nSee log for details.");

                install = filenames[0].left(filenames[0].lastIndexOf('/'));
                writeInstall(install);
            }
        }
    }
}
