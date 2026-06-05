#include "killservermanager.h"
#include "getadbdata.h"
#include "adbutils.h"

#include <QMessageBox>
#include <QTableWidget>
#include <QWidget>

KillServerManager::KillServerManager(QObject *parent)
    : QObject(parent)
{
}

void KillServerManager::killServer(QWidget *parentWidget, QTableWidget *deviceTable)
{
    QMessageBox msgBox(parentWidget);
    msgBox.setWindowTitle(QStringLiteral("Disconnect"));
    msgBox.setText(QStringLiteral("Disconnect all IPs?"));
    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msgBox.setWindowModality(Qt::WindowModal);
    if (msgBox.exec() == QMessageBox::Cancel)
        return;

    QString cstring = getadbpath() + " kill-server";
    getadbOutput(cstring);

    for (int row = 0; row < deviceTable->rowCount(); ++row) {
        QTableWidgetItem *descItem = deviceTable->item(row, 0);
        if (!descItem) continue;

        QTableWidgetItem *statusItem = deviceTable->item(row, 2);
        QString currentStatus = statusItem ? statusItem->text() : "";

        bool isUsb = !descItem->data(Qt::UserRole + 1).toString().isEmpty();

        if (currentStatus != "USB") {
            QString status = isUsb ? "USB" : "Disconnected";
            deviceTable->setItem(row, 2, new QTableWidgetItem(status));
        }
    }

    deviceTable->viewport()->update();
}
