#include "rebootmanager.h"
#include "adbutils.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QMessageBox>
#include <QTableWidget>
#include <QProcess>
#include <QEventLoop>
#include <QTimer>
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

        QProcess reboot_device;
        reboot_device.setProcessChannelMode(QProcess::MergedChannels);
        QStringList args = QProcess::splitCommand(adbPrefix + " reboot");
        reboot_device.start(args.takeFirst(), args);
        reboot_device.waitForStarted();
        QTimer timeoutTimer;
        timeoutTimer.setSingleShot(true);
        QEventLoop loop;
        QObject::connect(&reboot_device, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
        QObject::connect(&reboot_device, &QProcess::errorOccurred, &loop, &QEventLoop::quit);
        QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timeoutTimer.start(5000);
        loop.exec();
        if (reboot_device.state() != QProcess::NotRunning)
            reboot_device.kill();

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
}
