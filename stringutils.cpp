#include "stringutils.h"
#include <QProcess>
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

QString quotePosixArg(const QString &token)
{
    QString escaped = token;
    escaped.replace(QLatin1Char('\''), QLatin1String("'\\''"));
    return QLatin1Char('\'') + escaped + QLatin1Char('\'');
}

QString quoteCmdArg(const QString &token)
{
    return QLatin1Char('"') + token + QLatin1Char('"');
}

QChar cmdArgForbiddenChar(const QString &token)
{
    // cmd.exe expands %VAR% and !VAR! even inside double quotes and has no
    // escape for them; a quoted token cannot carry '"' either.
    for (const QChar ch : token) {
        if (ch == QLatin1Char('"') || ch == QLatin1Char('%')
            || ch == QLatin1Char('!') || ch == QLatin1Char('\n')
            || ch == QLatin1Char('\r'))
            return ch;
    }
    return QChar();
}

QString scrcpyCommandLine(const QString &binary, const QString &daddr,
                          const QString &argval, bool windows, QChar *forbidden)
{
    QStringList args;
    args << binary << QStringLiteral("-s")
         << (windows ? quoteCmdArg(daddr) : quotePosixArg(daddr));

    const QStringList tokens = QProcess::splitCommand(argval);
    for (const QString &token : tokens) {
        if (windows) {
            const QChar bad = cmdArgForbiddenChar(token);
            if (!bad.isNull()) {
                if (forbidden)
                    *forbidden = bad;
                return QString();
            }
            args << quoteCmdArg(token);
        } else {
            args << quotePosixArg(token);
        }
    }

    return args.join(QLatin1Char(' '));
}
