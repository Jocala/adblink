#include "stringutils.h"
#include <QRegularExpression>

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
