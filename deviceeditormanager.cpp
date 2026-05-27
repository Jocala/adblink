#include "deviceeditormanager.h"
#include "devicerecord.h"
#include "deviceeditor.h"
#include "getadbdata.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QStringList>
#include <QSqlDatabase>
#include <QTableWidget>
#include <QWidget>

DeviceEditorManager::DeviceEditorManager(QObject *parent)
    : QObject(parent)
{
}

void DeviceEditorManager::openEditor(QWidget *parentWidget, bool isNewRecord,
                                      QTableWidget *deviceTable, bool isKodiTab,
                                      const QString &version,
                                      QueryDeviceCallback queryDevice,
                                      EraseDbCallback eraseDb,
                                      ReloadTableCallback reloadTable)
{
    QString command = getadbOutput("null devices");
    QStringList mstringlist = command.split(QRegularExpression("[\t\n\r]"), Qt::SkipEmptyParts);
    QStringList dstringlist;

    if (command.contains("List of devices attached")) {
        mstringlist.removeFirst();
        for (int a = 0; a < mstringlist.size(); a = a + 2) {
            QStringList pieces = mstringlist.at(a).split(":", Qt::SkipEmptyParts);
            if (!mstringlist.at(a).contains("daemon"))
                dstringlist << pieces.at(0);
        }
    }

    DeviceRecord device;
    QString selectedDescription;

    if (!isNewRecord) {
        int selectedRow = deviceTable->currentRow();
        if (selectedRow >= 0 && deviceTable->item(selectedRow, 0)) {
            selectedDescription = deviceTable->item(selectedRow, 0)->text();
            device = queryDevice(selectedDescription);
        } else {
            QMessageBox::critical(parentWidget, "", "No device selected in table");
            return;
        }
    }

    DeviceEditor editor(parentWidget, isKodiTab, version, eraseDb);
    editor.setDeviceList(dstringlist);
    if (!isNewRecord)
        editor.setExistingDevice(device, selectedDescription);

    if (editor.exec()) {
        QSqlDatabase::database().commit();
        reloadTable();
    }
}
