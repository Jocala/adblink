#include "adbconnection.h"
#include "adbutils.h"
#include "logfile.h"

#include <QProcess>
#include <QRegularExpression>

AdbConnection::AdbConnection(QObject *parent)
    : QObject(parent)
    , m_adbPath(getadbpath())
{
}

QString AdbConnection::adbPath() const
{
    return m_adbPath;
}

bool AdbConnection::startServer()
{
    killServer();

    QString output = runCommand(QStringLiteral("start-server"));

    if (output.contains(QStringLiteral("daemon started successfully"))) {
        logfile(QStringLiteral("server started successfully"));
        return true;
    }

    logfile(QStringLiteral("start-server failed: ") + output);
    return false;
}

void AdbConnection::killServer()
{
    runCommand(QStringLiteral("kill-server"));
    logfile(QStringLiteral("server stopped"));
}

bool AdbConnection::isServerRunning() const
{
    QString output = runCommand(QStringLiteral("get-state"));
    return !output.trimmed().isEmpty();
}

QStringList AdbConnection::listDeviceSerialNumbers() const
{
    QString output = runCommand(QStringLiteral("devices"));
    QStringList lines = output.split(QRegularExpression(QStringLiteral("[\r\n]+")), Qt::SkipEmptyParts);
    QStringList serials;

    for (const QString &line : lines) {
        if (line.startsWith(QStringLiteral("List of devices")))
            continue;
        if (line.contains(QStringLiteral("device")))
            serials.append(line.section(QRegularExpression(QStringLiteral("\\s+")), 0, 0));
    }

    return serials;
}

QString AdbConnection::runCommand(const QString &args) const
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(m_adbPath, QStringList() << QProcess::splitCommand(args));
    process.waitForStarted();
    syncWaitForProcess(process);
    return QString::fromUtf8(process.readAll());
}

QString AdbConnection::runCommand(const QStringList &args) const
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(m_adbPath, args);
    process.waitForStarted();
    syncWaitForProcess(process);
    return QString::fromUtf8(process.readAll());
}

QString AdbConnection::adbPrefixForDevice(const DeviceRecord &device) const
{
    if (device.isusb)
        return QStringLiteral("\"%1\" -s %2").arg(m_adbPath, device.daddr);

    QString port = device.port.isEmpty() ? QStringLiteral("5555") : device.port;
    return QStringLiteral("\"%1\" -s %2:%3").arg(m_adbPath, device.daddr, port);
}

QString AdbConnection::shellForDevice(const DeviceRecord &device) const
{
    return adbPrefixForDevice(device) + QStringLiteral(" shell ");
}
