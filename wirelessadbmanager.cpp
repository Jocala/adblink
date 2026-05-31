#include "wirelessadbmanager.h"
#include "adbutils.h"
#include "devicerecord.h"
#include "getadbdata.h"
#include "logfile.h"
#include "tcpipdialog.h"

#include <QMessageBox>
#include <QTimer>
#include <QRegularExpression>
#include <QWidget>

WirelessAdbManager::WirelessAdbManager(QObject *parent)
    : QObject(parent)
{
}

void WirelessAdbManager::enableWirelessAdb(QWidget *parentWidget,
                                            const DeviceRecord &device,
                                            const QString &adbPrefix)
{
    if (!device.isusb) {
        QMessageBox::critical(parentWidget, "", "USB devices only!");
        return;
    }

    QString cstring;
    QString command;

    cstring = adbPrefix + " shell ip route";
    command = getadbOutput(cstring);

    QString ip;
    {
        QRegularExpression re(R"(src\s+(\d+\.\d+\.\d+\.\d+))");
        QRegularExpressionMatch match = re.match(command);
        if (match.hasMatch())
            ip = match.captured(1);
    }

    logfile("Device IP: " + ip);

    tcpipDialog dialog;

    cstring = adbPrefix + " shell getprop persist.adb.tcp.port";
    command = getadbOutput(cstring);
    logfile("shell getprop persist.adb.tcp.port: " + command);

    dialog.settcplabel("Device IP: " + ip);

    if (dialog.exec() == QDialog::Accepted) {
        cstring = adbPrefix + " tcpip 5555";
        command = getadbOutput(cstring);
        logfile(command);

        QTimer::singleShot(2000, this, [parentWidget, ip]() {
            QString cstring = getadbpath() + " connect " + ip + ":5555";
            QString command = getadbOutput(cstring);
            logfile("adb connect: " + command);

            if (command.contains("connected to")) {
                QMessageBox::information(parentWidget, "Success",
                                         "Wireless ADB enabled for " + ip);

                cstring = getadbpath() + " disconnect " + ip + ":5555";
                command = getadbOutput(cstring);
                logfile("adb disconnect: " + command);
            } else {
                QMessageBox::warning(parentWidget, "Failure",
                                     "Failed to connect over TCP/IP to " + ip);
            }
        });
    }
}
