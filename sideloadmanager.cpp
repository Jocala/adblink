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

    QFileDialog dialog(parentWidget, tr("APK files (*.apk);;All files (.*)"), install);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("APK files (*.apk);;All files (.*)"));
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    QStringList filenames;
    if (dialog.exec())
        filenames = dialog.selectedFiles();

    if (!filenames.isEmpty()) {
        QMessageBox msgBox(parentWidget);
        msgBox.setWindowTitle(QStringLiteral("Install"));
        msgBox.setText(QStringLiteral("Install APKs?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowModality(Qt::WindowModal);
        if (msgBox.exec() == QMessageBox::Yes) {
            logfile("starting APK installation(s)");
            for (int i = 0; i < filenames.count(); i++)
                installer = installApk(filenames.at(i));

            if (installer) {
                writeInstall(filenames[0].left(filenames[0].lastIndexOf('/')));
                QMessageBox msgBox(parentWidget);
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(QString());
                msgBox.setText(QStringLiteral("APK(s) installed.\nSee log for details."));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setWindowModality(Qt::WindowModal);
                msgBox.exec();

                install = filenames[0].left(filenames[0].lastIndexOf('/'));
                writeInstall(install);
            }
        }
    }
}
