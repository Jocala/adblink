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
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("No file selected"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    if (!isPackageInstalled(adbPrefix, package)) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("%1 not installed").arg(package));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        logfile("Error: " + package + " not installed");
        return;
    }

    QMessageBox msgBox(parentWidget);
    msgBox.setWindowTitle(QStringLiteral("Uninstall"));
    msgBox.setText(QStringLiteral("Uninstall %1?").arg(package));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setWindowModality(Qt::WindowModal);
    if (msgBox.exec() == QMessageBox::Yes) {
        QString cstring;
        if (!keepbox)
            cstring = adbPrefix + " shell pm uninstall " + package;
        else
            cstring = adbPrefix + " shell pm uninstall -k " + package;

        logfile("uninstall: " + cstring);

        QString command = runLongProcess(cstring, "Uninstall APK");

        if (!command.contains("Success")) {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Uninstall failed"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            logfile(package + " uninstalled");
        } else {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Uninstalled"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            logfile(package + " uninstalled");
        }
    }
}
