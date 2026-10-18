#include "stringutils.h"
#include <QRegularExpression>
#include <QStringList>

QString stripString(const QString &str)
{
    QString result = str.simplified();
    result.replace(QLatin1String(" "), QLatin1String(""));
    return result;
}

QString checkslash(const QString &qpath)
{
    QString result = qpath;
    if (!result.startsWith(QLatin1Char('/')))
        result.prepend(QLatin1Char('/'));
    if (!result.endsWith(QLatin1Char('/')))
        result.append(QLatin1Char('/'));
    return result;
}

bool validateIPAddress(const QString &ipAddress)
{
    QString normalized = ipAddress.trimmed();

    static const QRegularExpression ipRegex(
        QStringLiteral(
            "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
            "|"
            "^[a-zA-Z0-9][a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9]?"
            "(\\.[a-zA-Z0-9][a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9]?)*"
            "\\.[a-zA-Z0-9]{1,}$"),
        QRegularExpression::CaseInsensitiveOption);

    return ipRegex.match(normalized).hasMatch();
}

QString usbStatus(const QHash<QString, QString> &cache, const QString &daddr)
{
    auto it = cache.find(daddr);
    if (it == cache.end())
        return QStringLiteral("Disconnected");
    if (it.value() == QLatin1String("device"))
        return QStringLiteral("Connected");
    QString status = it.value();
    if (!status.isEmpty())
        status[0] = status[0].toUpper();
    return status;
}

bool isConnectedStatus(const QString &status)
{
    static const QStringList connectedStates = {
        QStringLiteral("connected"),
        QStringLiteral("recovery"),
        QStringLiteral("rescue")
    };
    return connectedStates.contains(status.trimmed().toLower());
}

namespace {

bool isHexLengthPrefix(const QByteArray &prefix)
{
    for (char c : prefix) {
        const bool hex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        if (!hex)
            return false;
    }
    return true;
}

bool isDeviceListFrame(const QByteArray &payload)
{
    if (payload.isEmpty())
        return true;
    if (!payload.endsWith('\n'))
        return false;
    for (const QByteArray &rawLine : payload.split('\n')) {
        const QByteArray line = rawLine.trimmed();
        if (!line.isEmpty() && !line.contains('\t'))
            return false;
    }
    return true;
}

} // namespace

QList<QPair<QString, QString>> parseTrackDevices(QByteArray &buffer)
{
    QList<QPair<QString, QString>> entries;

    // adb on Windows relays the list through a text-mode stdout, so every newline
    // arrives as \r\n while the frame header counts the \n only. Dropping the
    // carriage returns keeps the declared lengths aligned with the wire.
    buffer.replace("\r", "");

    while (buffer.size() >= 4) {
        const QByteArray prefix = buffer.left(4);
        if (!isHexLengthPrefix(prefix)) {
            // not a length prefix; drop one byte and look for the next frame
            buffer.remove(0, 1);
            continue;
        }
        const int payloadLength = prefix.toInt(nullptr, 16);
        if (buffer.size() < 4 + payloadLength)
            break;

        const QByteArray payload = buffer.mid(4, payloadLength);
        if (!isDeviceListFrame(payload)) {
            // a length that parsed but does not frame a device list; resync
            buffer.remove(0, 1);
            continue;
        }
        buffer.remove(0, 4 + payloadLength);

        const QList<QByteArray> lines = payload.split('\n');
        for (const QByteArray &rawLine : lines) {
            const QByteArray line = rawLine.trimmed();
            if (line.isEmpty())
                continue;
            const int tab = line.indexOf('\t');
            if (tab < 0)
                continue;
            const QString serial = QString::fromLatin1(line.left(tab)).trimmed();
            if (serial.isEmpty())
                continue;
            entries.append(qMakePair(serial,
                QString::fromLatin1(line.mid(tab + 1)).trimmed()));
        }
    }

    return entries;
}
