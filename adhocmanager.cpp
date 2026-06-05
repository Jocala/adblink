#include "adhocmanager.h"
#include "logfile.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QLineEdit>
#include <QWidget>

AdhocManager::AdhocManager(QObject *parent)
    : QObject(parent)
{
}

void AdhocManager::createAdhocRecord(QWidget *parentWidget, QLineEdit *adhocIpEdit,
                                      ReloadTableCallback reloadTable)
{
    if (!adhocIpEdit->text().isEmpty()) {
        QString adhocIPText = adhocIpEdit->text().trimmed();
        int colonIndex = adhocIPText.indexOf(':');
        QString daddr, port;

        if (colonIndex != -1) {
            daddr = adhocIPText.left(colonIndex).trimmed();
            port = adhocIPText.mid(colonIndex + 1).trimmed();
        } else {
            daddr = adhocIPText;
            port = "5555";
        }

        bool ok;
        int portNum = port.toInt(&ok);
        if (!ok || portNum < 1 || portNum > 65535) {
            logfile("Invalid port: " + port);
            QMessageBox msgBox(parentWidget);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Invalid port: %1").arg(port));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            return;
        }

        QSqlQuery query;
        query.prepare("INSERT OR REPLACE INTO device (description, daddr, port, isusb, data_root, xbmcpackage, filepath) "
                      "VALUES (:description, :daddr, :port, :isusb, :data_root, :xbmcpackage, :filepath)");
        query.bindValue(":description", "Ad hoc");
        query.bindValue(":daddr", daddr);
        query.bindValue(":port", port);
        query.bindValue(":isusb", 0);
        query.bindValue(":data_root", "/sdcard/");
        query.bindValue(":xbmcpackage", "org.xbmc.kodi");
        query.bindValue(":filepath", "/files/.kodi");
        if (!query.exec()) {
            logfile("Failed to insert temporary device: " + query.lastError().text());
        } else {
            logfile("Temporary device record inserted: Ad hoc IP, " + daddr + ":" + port);
        }
    }

    reloadTable();
}
