#ifndef DEVICEEDITORMANAGER_H
#define DEVICEEDITORMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

class QTableWidget;
struct DeviceRecord;

class DeviceEditorManager : public QObject
{
    Q_OBJECT

public:
    using QueryDeviceCallback = std::function<DeviceRecord(const QString&)>;
    using EraseDbCallback = std::function<void()>;
    using ReloadTableCallback = std::function<void()>;

    explicit DeviceEditorManager(QObject *parent = nullptr);

    void openEditor(QWidget *parentWidget, bool isNewRecord,
                    QTableWidget *deviceTable, bool isKodiTab,
                    const QString &version,
                    QueryDeviceCallback queryDevice,
                    EraseDbCallback eraseDb,
                    ReloadTableCallback reloadTable);
};

#endif // DEVICEEDITORMANAGER_H
