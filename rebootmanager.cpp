#include "rebootmanager.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QMessageBox>
#include <QTableWidget>
#include <QProcess>
#include <QElapsedTimer>
#include <QApplication>
#include <QWidget>

RebootManager::RebootManager(QObject *parent)
    : QObject(parent)
{
}

void RebootManager::rebootDevice(QWidget *parentWidget, QTableWidget *deviceTable,
                                  bool isUsb, const QString &adbPrefix)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parentWidget, "Reboot Device", "Reboot Device?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        logfile("rebooting device");

        QElapsedTimer rtimer;
        rtimer.start();
        QProcess reboot_device;
        reboot_device.setProcessChannelMode(QProcess::MergedChannels);
        reboot_device.start(adbPrefix + " reboot");
        reboot_device.waitForStarted();
        while (reboot_device.state() != QProcess::NotRunning) {
            qApp->processEvents();
            if (rtimer.elapsed() >= 5000)
                break;
        }

        int selectedRow = deviceTable->currentRow();
        QString daddr = deviceTable->item(selectedRow, 1)->text();

        if (!isUsb) {
            QString cstring = "null disconnect " + daddr;
            QString command = getadbOutput(cstring);
            logfile(command);
            logfile("disconnect: " + daddr);
        }

        if (selectedRow >= 0 && deviceTable->item(selectedRow, 2)) {
            deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Disconnected"));
        }
    }
}
