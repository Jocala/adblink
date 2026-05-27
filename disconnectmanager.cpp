#include "disconnectmanager.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QMessageBox>
#include <QTableWidget>
#include <QWidget>

DisconnectManager::DisconnectManager(QObject *parent)
    : QObject(parent)
{
}

void DisconnectManager::disconnectDevice(QWidget *parentWidget, QTableWidget *deviceTable)
{
    int selectedRow = deviceTable->currentRow();

    if (selectedRow < 0 || !deviceTable->item(selectedRow, 2) || !deviceTable->item(selectedRow, 1)) {
        QMessageBox::critical(parentWidget, "", "No valid device selected");
        return;
    }
    if (deviceTable->item(selectedRow, 2)->text() != "Connected") {
        QMessageBox::critical(parentWidget, "", "Selected device is not connected");
        return;
    }

    if (deviceTable->item(selectedRow, 1)->text().contains("USB")) {
        QMessageBox::critical(parentWidget, "", "Inactive for USB connections");
        return;
    }

    QString daddr = deviceTable->item(selectedRow, 1)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parentWidget, "Disconnect", "Disconnect device?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    QString cstring = "null disconnect " + daddr;
    QString command = getadbOutput(cstring);

    logfile(command);
    logfile("disconnect: " + daddr);

    if (selectedRow >= 0 && deviceTable->item(selectedRow, 2)) {
        deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Disconnected"));
    }
}
