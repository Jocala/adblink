#ifndef ADBDEVICE_H
#define ADBDEVICE_H

#include <QObject>
#include <QStringList>

#include "devicerecord.h"

class AdbDevice : public QObject
{
    Q_OBJECT

public:
    explicit AdbDevice(const DeviceRecord &device, QObject *parent = nullptr);

    const DeviceRecord &deviceRecord() const { return m_device; }
    void setDeviceRecord(const DeviceRecord &device) { m_device = device; }

    QString adbPrefix() const;
    QString shellPrefix() const;

    bool isPackageInstalled(const QString &packageName) const;
    bool fileExists(const QString &path) const;
    bool isSuAvailable() const;
    bool mountSystem(const QString &mountOption) const;
    bool reboot(const QString &rebootMode) const;
    bool installApk(const QString &apkPath) const;

    QString runShell(const QString &shellCommand) const;
    QString runAdb(const QString &adbArgs) const;
    QString runCommand(const QString &binary, const QStringList &args) const;

    QString batteryLevel() const;
    QString androidVersion() const;
    QString manufacturer() const;
    QString deviceName() const;
    QString deviceRelease() const;

    bool screenCapture(const QString &destinationDir, const QString &fileName);
    QString pullFile(const QString &remotePath, const QString &localDir) const;
    QString pushFile(const QString &localPath, const QString &remotePath) const;
    void removeFile(const QString &remotePath) const;

signals:
    void logMessage(const QString &message) const;

private:
    DeviceRecord m_device;
};

#endif // ADBDEVICE_H
