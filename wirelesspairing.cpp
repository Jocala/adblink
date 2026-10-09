#include "wirelesspairing.h"
#include "adbutils.h"
#include "stringutils.h"

#include <QProcess>
#include <QRegularExpression>

namespace WirelessPairing {

static bool isPort(const QString &s)
{
    bool ok = false;
    const int port = s.trimmed().toInt(&ok);
    return ok && port >= 1 && port <= 65535;
}

QString validate(const Request &req)
{
    const QString host = req.host.trimmed();
    // validateIPAddress() also accepts host names, which lets "192.168.1.300"
    // through as a "name"; all-numeric input must be a real IPv4 address.
    static const QRegularExpression numericRe(QStringLiteral("^[\\d.]+$"));
    static const QRegularExpression ipv4Re(QStringLiteral(
        "^((25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)\\.){3}(25[0-5]|2[0-4]\\d|1\\d\\d|[1-9]?\\d)$"));
    if (!validateIPAddress(host)
        || (numericRe.match(host).hasMatch() && !ipv4Re.match(host).hasMatch()))
        return QStringLiteral("Enter the device's IP address, e.g. 192.168.1.20.");
    if (!isPort(req.pairPort))
        return QStringLiteral("Enter the pairing port shown under \"Pair device with pairing code\".");
    static const QRegularExpression codeRe(QStringLiteral("^\\d{6}$"));
    if (!codeRe.match(req.code.trimmed()).hasMatch())
        return QStringLiteral("The pairing code is the 6 digits shown on the device.");
    if (!req.connectPort.trimmed().isEmpty() && !isPort(req.connectPort))
        return QStringLiteral("The connect port must be a number from 1 to 65535.");
    return QString();
}

QStringList pairArguments(const Request &req)
{
    return { QStringLiteral("pair"),
             req.host.trimmed() + QLatin1Char(':') + req.pairPort.trimmed(),
             req.code.trimmed() };
}

QStringList connectArguments(const Request &req)
{
    return { QStringLiteral("connect"),
             req.host.trimmed() + QLatin1Char(':') + req.connectPort.trimmed() };
}

bool pairSucceeded(const QString &adbOutput)
{
    // "Successfully paired to 192.168.1.20:37099 [guid=adb-XXXX-abc]"
    return adbOutput.contains(QStringLiteral("Successfully paired"), Qt::CaseInsensitive);
}

bool connectSucceeded(const QString &adbOutput)
{
    // "connected to 192.168.1.20:41235" / "already connected to ..."
    // Failure: "failed to connect to ..." / "cannot connect to ..."
    if (adbOutput.contains(QStringLiteral("failed"), Qt::CaseInsensitive)
        || adbOutput.contains(QStringLiteral("cannot"), Qt::CaseInsensitive))
        return false;
    return adbOutput.contains(QStringLiteral("connected to"), Qt::CaseInsensitive);
}

static QString runAdb(const QString &adbPath, const QStringList &args, int timeoutMs)
{
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(adbPath, args);
    if (!p.waitForStarted(5000))
        return QStringLiteral("error: could not start adb at ") + adbPath;
    syncWaitForProcess(p, timeoutMs);
    QString out = QString::fromUtf8(p.readAll());
    if (p.exitStatus() == QProcess::CrashExit && out.trimmed().isEmpty())
        out = QStringLiteral("error: timed out waiting for the device");
    return out;
}

Result run(const QString &adbPath, const Request &req, int timeoutMs)
{
    Result r;
    r.pairOutput = runAdb(adbPath, pairArguments(req), timeoutMs);
    r.paired = pairSucceeded(r.pairOutput);
    if (r.paired && !req.connectPort.trimmed().isEmpty()) {
        r.connectOutput = runAdb(adbPath, connectArguments(req), timeoutMs);
        r.connected = connectSucceeded(r.connectOutput);
    }
    return r;
}

QString describeFailure(const QString &adbOutput)
{
    QString msg = adbOutput.trimmed();
    const QStringList lines = msg.split(QRegularExpression(QStringLiteral("[\r\n]+")),
                                        Qt::SkipEmptyParts);
    msg = lines.isEmpty() ? QString() : lines.last().trimmed();
    msg.remove(QRegularExpression(QStringLiteral("^(adb: )?(error: )?"), QRegularExpression::CaseInsensitiveOption));
    if (msg.isEmpty())
        return QStringLiteral("No response from adb.");
    if (msg.contains(QStringLiteral("Wrong password"), Qt::CaseInsensitive)
        || msg.contains(QStringLiteral("protocol fault"), Qt::CaseInsensitive))
        return QStringLiteral("The pairing code was not accepted. Codes expire quickly - "
                              "open the pairing popup again and use the new code.");
    if (msg.contains(QStringLiteral("Unable to start pairing client"), Qt::CaseInsensitive)
        || msg.contains(QStringLiteral("Connection refused"), Qt::CaseInsensitive))
        return QStringLiteral("Could not reach the pairing port. Check the IP address and "
                              "port, and keep the pairing popup open on the device.");
    return msg;
}

} // namespace WirelessPairing
