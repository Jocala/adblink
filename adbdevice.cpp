#include "adbdevice.h"
#include "adbutils.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>

AdbDevice::AdbDevice(const DeviceRecord &device, QObject *parent)
    : QObject(parent)
    , m_device(device)
{
}

QString AdbDevice::adbPrefix() const
{
    QString adbPath = getadbpath();
    if (m_device.isusb)
        return QStringLiteral("\"%1\" -s %2").arg(adbPath, m_device.daddr);
    QString port = m_device.port.isEmpty() ? QStringLiteral("5555") : m_device.port;
    return QStringLiteral("\"%1\" -s %2:%3").arg(adbPath, m_device.daddr, port);
}

QString AdbDevice::shellPrefix() const
{
    return adbPrefix() + QStringLiteral(" shell ");
}

QString AdbDevice::runCommand(const QString &binary, const QStringList &args) const
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(binary, args);
    process.waitForStarted();
    syncWaitForProcess(process);
    return QString::fromUtf8(process.readAll());
}

QString AdbDevice::runShell(const QString &shellCommand) const
{
    return runCommand(getadbpath(), QStringList()
        << QStringLiteral("-s") << m_device.daddr
        << QStringLiteral("shell") << shellCommand);
}

QString AdbDevice::runAdb(const QString &adbArgs) const
{
    return runCommand(getadbpath(), QStringList()
        << QStringLiteral("-s") << m_device.daddr
        << QProcess::splitCommand(adbArgs));
}

bool AdbDevice::isPackageInstalled(const QString &packageName) const
{
    QString output = runShell(QStringLiteral("pm list packages "));
    return output.contains(packageName);
}

bool AdbDevice::fileExists(const QString &path) const
{
    // Check via ls
    QString output = runShell(QStringLiteral("ls ") + path);
    return !output.contains(QStringLiteral("No such file")) && !output.isEmpty();
}

bool AdbDevice::isSuAvailable() const
{
    QString output = runShell(QStringLiteral("/data/local/tmp/adblink/which su"));
    return output.contains(QStringLiteral("su"));
}

bool AdbDevice::mountSystem(const QString &mountOption) const
{
    QString output = runShell(QStringLiteral("su -c /data/local/tmp/adblink/mount -o ")
        + mountOption + QStringLiteral(",remount /"));
    return output.isEmpty();
}

bool AdbDevice::reboot(const QString &rebootMode) const
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    QStringList args = QProcess::splitCommand(adbPrefix() + QStringLiteral(" ") + rebootMode);
    process.start(args.takeFirst(), args);
    process.waitForStarted();
    syncWaitForProcess(process, 5000);
    return true;
}

bool AdbDevice::installApk(const QString &apkPath) const
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(QStringLiteral("\"%1\"").arg(getadbpath()), QStringList()
        << QStringLiteral("-s") << m_device.daddr
        << QStringLiteral("install") << QStringLiteral("-r")
        << apkPath);
    process.waitForStarted();
    syncWaitForProcess(process);

    QString output = QString::fromUtf8(process.readAll());
    return output.contains(QStringLiteral("uccess")) && !output.contains(QStringLiteral("Failure"));
}

QString AdbDevice::batteryLevel() const
{
    QString output = runShell(QStringLiteral("dumpsys battery"));
    if (output.contains(QStringLiteral("level"))) {
        QStringList lines = output.split(QRegularExpression(QStringLiteral("[\r\n]+")), Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            if (line.contains(QStringLiteral("level"))) {
                return line.section(QStringLiteral(":"), 1, 1).trimmed();
            }
        }
    }
    return QString();
}

QString AdbDevice::androidVersion() const
{
    QString output = runShell(QStringLiteral("getprop ro.build.version.sdk"));
    return output.trimmed();
}

QString AdbDevice::manufacturer() const
{
    QString output = runShell(QStringLiteral("getprop ro.product.manufacturer"));
    return output.trimmed();
}

QString AdbDevice::deviceName() const
{
    QString output = runShell(QStringLiteral("getprop ro.product.model"));
    return output.trimmed();
}

QString AdbDevice::deviceRelease() const
{
    QString output = runShell(QStringLiteral("getprop ro.build.version.release"));
    return output.trimmed();
}

bool AdbDevice::screenCapture(const QString &destinationDir, const QString &fileName)
{
    QString remotePath = QStringLiteral("/data/local/tmp/") + fileName;

    QString cmd = QStringLiteral("screencap -p ") + remotePath;
    QString output = runShell(cmd);

    if (!output.isEmpty())
        return false;

    pullFile(remotePath, destinationDir);
    removeFile(remotePath);

    QString localFilePath = destinationDir + QStringLiteral("/") + fileName;
    return QFileInfo::exists(localFilePath);
}

QString AdbDevice::pullFile(const QString &remotePath, const QString &localDir) const
{
    return runAdb(QStringLiteral("pull ") + remotePath + QStringLiteral(" ") + localDir);
}

QString AdbDevice::pushFile(const QString &localPath, const QString &remotePath) const
{
    return runAdb(QStringLiteral("push ") + localPath + QStringLiteral(" ") + remotePath);
}

void AdbDevice::removeFile(const QString &remotePath) const
{
    runShell(QStringLiteral("rm ") + remotePath);
}
