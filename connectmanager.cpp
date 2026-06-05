#include "connectmanager.h"
#include "devicerecord.h"
#include "connectadb.h"
#include "logfile.h"

#include <QMessageBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QWidget>

ConnectManager::ConnectManager(QObject *parent)
    : QObject(parent)
{
}

void ConnectManager::connectToDevice(QWidget *parentWidget,
                                      QLineEdit *adhocIpEdit,
                                      QTableWidget *deviceTable,
                                      DeviceQueryCallback queryDevice,
                                      ValidateIPCallback validateIP,
                                      InfoLogCallback infoLog,
                                      AdhocIpCallback adhocIpHandler,
                                      const QString &adbPath)
{
    const QString adhoc = "Ad hoc";

    if (!adhocIpEdit->text().isEmpty()) {
        adhocIpHandler();
        adhocIpEdit->clear();
        for (int row = 0; row < deviceTable->rowCount(); ++row) {
            QTableWidgetItem *item = deviceTable->item(row, 0);
            if (item && item->text() == adhoc) {
                deviceTable->selectRow(row);
                break;
            }
        }
    }

    int selectedRow = deviceTable->currentRow();
    QString selectedDescription;
    if (selectedRow >= 0 && deviceTable->item(selectedRow, 0)) {
        selectedDescription = deviceTable->item(selectedRow, 0)->text();
    } else {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("No device selected in table"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    DeviceRecord device = queryDevice(selectedDescription);

    if (device.isusb) {
        logfile("USB connection attempted, not supported");
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Inactive for USB connections"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    if (!validateIP(device.daddr)) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QStringLiteral("Error"));
        msgBox.setText(QStringLiteral("Invalid IP address"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    QString port = device.port.isEmpty() ? "5555" : device.port;
    QString daddr = device.daddr + ":" + port;

    QString command = connectadb(adbPath, QStringList() << "connect" << daddr);

    if (command.contains("failed to authenticate") || command.contains("offline")) {
        deviceTable->setItem(selectedRow, 2, new QTableWidgetItem(
            command.contains("failed to authenticate") ? "Unauthorized" : "Offline"));
        logfile(command);
        connectadb(adbPath, QStringList() << "disconnect" << daddr);
        return;
    }

    if (command.contains("connected to")) {
        deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Connected"));
        deviceTable->clearSelection();
        deviceTable->setCurrentCell(selectedRow, 0);
        deviceTable->selectRow(selectedRow);
        deviceTable->setFocus();
        logfile("Connected to " + daddr);
        infoLog();
    } else {
        deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("NA"));
        logfile("Unable to connect to: " + daddr);
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Unable to connect to: %1").arg(daddr));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
    }
}
