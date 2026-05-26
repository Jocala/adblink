#ifndef DEVICEEDITOR_H
#define DEVICEEDITOR_H

#include <QObject>
#include <QStringList>
#include <functional>
#include "devicerecord.h"

class QWidget;

class DeviceEditor : public QObject
{
    Q_OBJECT

public:
    explicit DeviceEditor(QWidget *parent, bool iskodi,
                         const QString &version,
                         std::function<void()> eraseCallback);

    void setDeviceList(const QStringList &devices);
    void setExistingDevice(const DeviceRecord &device, const QString &description);

    bool exec();

private:
    QWidget *m_parent;
    bool m_iskodi;
    QString m_version;
    QStringList m_devices;
    DeviceRecord m_existingDevice;
    QString m_existingDescription;
    bool m_isUpdate = false;
    std::function<void()> m_eraseCallback;
};

#endif // DEVICEEDITOR_H
