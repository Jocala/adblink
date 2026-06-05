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
        QMessageBox msgBox(parentWidget);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("No device selected in table"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return;
    }

    if (!descrip.isEmpty()) {
        if (deviceTable->item(selectedRow, 2) &&
            deviceTable->item(selectedRow, 2)->text() == "Connected") {
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Cannot delete \"%1\" while connected").arg(descrip));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            return;
        }

        QMessageBox msgBox(parentWidget);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Delete %1?").arg(descrip));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setWindowModality(Qt::WindowModal);
        if (msgBox.exec() == QMessageBox::No)
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
