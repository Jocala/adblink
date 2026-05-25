#ifndef ADBCONNECTION_H
#define ADBCONNECTION_H

#include <QObject>
#include <QStringList>

#include "devicerecord.h"

class AdbConnection : public QObject
{
    Q_OBJECT

public:
    explicit AdbConnection(QObject *parent = nullptr);

    QString adbPath() const;

    bool startServer();
    void killServer();
    bool isServerRunning() const;

    QStringList listDeviceSerialNumbers() const;

    QString runCommand(const QString &args) const;
    QString runCommand(const QStringList &args) const;

    QString adbPrefixForDevice(const DeviceRecord &device) const;
    QString shellForDevice(const DeviceRecord &device) const;

signals:
    void logMessage(const QString &message) const;

private:
    QString m_adbPath;
};

#endif // ADBCONNECTION_H
