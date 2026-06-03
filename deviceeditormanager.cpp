#include "deviceeditormanager.h"
#include "adbutils.h"
#include "devicerecord.h"
#include "deviceeditor.h"

#include <QMessageBox>
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
    if (!isNewRecord)
        editor.setExistingDevice(device, selectedDescription);

    if (editor.exec()) {
        QSqlDatabase::database().commit();
        reloadTable();
    }
}
