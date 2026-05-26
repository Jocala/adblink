#ifndef WIRELESSADBMANAGER_H
#define WIRELESSADBMANAGER_H

#include <QObject>
#include <QString>

struct DeviceRecord;

class WirelessAdbManager : public QObject
{
    Q_OBJECT

public:
    explicit WirelessAdbManager(QObject *parent = nullptr);

    void enableWirelessAdb(QWidget *parentWidget, const DeviceRecord &device,
                           const QString &adbPrefix);
};

#endif // WIRELESSADBMANAGER_H
