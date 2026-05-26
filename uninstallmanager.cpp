#include "uninstallmanager.h"
#include "devicerecord.h"
#include "uninstalldialog.h"
#include "adbutils.h"
#include "logfile.h"

#include <QMessageBox>
#include <QWidget>

UninstallManager::UninstallManager(QObject *parent)
    : QObject(parent)
{
}

void UninstallManager::uninstallPackage(QWidget *parentWidget,
                                        const DeviceRecord &device,
                                        const QString &adbPrefix,
                                        RunLongProcessCallback runLongProcess)
{
    logfile("open uninstall dialog");

    uninstallDialog dialog(device.daddr, "", parentWidget);
    dialog.setWindowModality(Qt::WindowModal);

    QString package;
    bool keepbox = false;

    if (dialog.exec() == QDialog::Accepted) {
        package = dialog.packageName();
        keepbox = dialog.keepBox();
    } else {
        return;
    }

    if (package.isEmpty()) {
        QMessageBox::critical(parentWidget, "", "No file selected");
        return;
    }

    if (!isPackageInstalled(adbPrefix, package)) {
        QMessageBox::critical(parentWidget, "", package + " not installed");
        logfile("Error: " + package + " not installed");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parentWidget, "Uninstall",
                                  "Uninstall " + package + "?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString cstring;
        if (!keepbox)
            cstring = adbPrefix + " shell pm uninstall " + package;
        else
            cstring = adbPrefix + " shell pm uninstall -k " + package;

        logfile("uninstall: " + cstring);

        QString command = runLongProcess(cstring, "Uninstall APK");

        if (!command.contains("Success")) {
            QMessageBox::critical(parentWidget, "", "Uninstall failed");
            logfile(package + " uninstalled");
        } else {
            QMessageBox::information(parentWidget, "", "Uninstalled");
            logfile(package + " uninstalled");
        }
    }
}
