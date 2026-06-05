#include "disconnectmanager.h"
#include "adbutils.h"
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
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("No valid device selected"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }
    if (deviceTable->item(selectedRow, 2)->text() != "Connected") {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Selected device is not connected"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    if (deviceTable->item(selectedRow, 1)->text().contains("USB")) {
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Inactive for USB connections"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    QString daddr = deviceTable->item(selectedRow, 1)->text();

    QMessageBox msgBox(parentWidget);
    msgBox.setWindowTitle("Disconnect");
    msgBox.setText("Disconnect device?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setWindowModality(Qt::WindowModal);
    if (msgBox.exec() == QMessageBox::No)
        return;

    QString cstring = getadbpath() + " disconnect " + daddr;
    QString command = getadbOutput(cstring);

    logfile(command);
    logfile("disconnect: " + daddr);

    if (selectedRow >= 0 && deviceTable->item(selectedRow, 2)) {
        deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Disconnected"));
    }
}
