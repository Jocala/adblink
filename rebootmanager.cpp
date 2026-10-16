#include "rebootmanager.h"
#include "adbutils.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QMessageBox>
#include <QTableWidget>
#include <QProcess>
#include <QWidget>

RebootManager::RebootManager(QObject *parent)
    : QObject(parent)
{
}

void RebootManager::rebootDevice(QWidget *parentWidget, QTableWidget *deviceTable,
                                  bool isUsb, const QString &adbPrefix,
                                  const QString &mode)
{
    QMessageBox msgBox(parentWidget);
    msgBox.setWindowTitle(QStringLiteral("Reboot Device"));
    msgBox.setText(mode.isEmpty()
                       ? QStringLiteral("Reboot Device?")
                       : QStringLiteral("Reboot device into %1 mode?").arg(mode));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setWindowModality(Qt::WindowModal);
    if (msgBox.exec() != QMessageBox::Yes)
        return;
        logfile(mode.isEmpty() ? "rebooting device"
                               : "rebooting device into " + mode);

        QProcess reboot_device;
        reboot_device.setProcessChannelMode(QProcess::MergedChannels);
        QStringList args = QProcess::splitCommand(
            adbPrefix + " reboot" + (mode.isEmpty() ? QString() : " " + mode));
        reboot_device.start(args.takeFirst(), args);
        reboot_device.waitForStarted();
        syncWaitForProcess(reboot_device, 5000);

        int selectedRow = deviceTable->currentRow();
        QString daddr = deviceTable->item(selectedRow, 1)->text();

        if (!isUsb) {
            QString cstring = getadbpath() + " disconnect " + daddr;
            QString command = getadbOutput(cstring);
            logfile(command);
            logfile("disconnect: " + daddr);
        }

        if (selectedRow >= 0 && deviceTable->item(selectedRow, 2)) {
            deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Disconnected"));
        }
}
