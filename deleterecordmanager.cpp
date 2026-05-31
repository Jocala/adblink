#include "deleterecordmanager.h"
#include "getadbdata.h"
#include "adbutils.h"
#include "logfile.h"

#include <QMessageBox>
#include <QTableWidget>
#include <QWidget>

DeleteRecordManager::DeleteRecordManager(QObject *parent)
    : QObject(parent)
{
}

void DeleteRecordManager::deleteSelectedDevice(QWidget *parentWidget,
                                                QTableWidget *deviceTable,
                                                DeleteRecordCallback deleteRecord)
{
    QString descrip;
    QString daddr;

    int selectedRow = deviceTable->currentRow();
    if (selectedRow >= 0 && deviceTable->item(selectedRow, 0)) {
        descrip = deviceTable->item(selectedRow, 0)->text();
    } else {
        QMessageBox::critical(parentWidget, "", "No device selected in table");
        return;
    }

    if (!descrip.isEmpty()) {
        if (deviceTable->item(selectedRow, 2) &&
            deviceTable->item(selectedRow, 2)->text() == "Connected") {
            QMessageBox::warning(parentWidget, "", "Cannot delete \"" + descrip + "\" while connected");
            return;
        }

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(parentWidget, "", "Delete " + descrip + "?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
            return;

        deleteRecord(descrip);

        selectedRow = deviceTable->currentRow();
        if (selectedRow < 0 || !deviceTable->item(selectedRow, 1)) {
            logfile("Device table was reset during delete operation");
            return;
        }
        daddr = deviceTable->item(selectedRow, 1)->text();

        QString cstring = getadbpath() + " disconnect " + daddr;
        QString command = getadbOutput(cstring);
        logfile(command);
        logfile("disconnect: " + daddr);

        deviceTable->removeRow(selectedRow);
        logfile(descrip + " is deleted");
    }
}
