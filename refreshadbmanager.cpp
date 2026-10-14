#include "refreshadbmanager.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QRegularExpression>
#include <QStringList>

RefreshAdbManager::RefreshAdbManager(QObject *parent)
    : QObject(parent)
{
}

QHash<QString, QString> RefreshAdbManager::parseAdbDevices(const QString &output)
{
    QHash<QString, QString> statuses;
    const QStringList lines = output.split(QLatin1Char('\n'));
    for (QString line : lines) {
        line = line.trimmed();
        if (line.isEmpty())
            continue;
        if (line.startsWith(QStringLiteral("List of devices")))
            continue;
        if (line.startsWith(QLatin1Char('*')))
            continue;
        const QStringList parts = line.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
        if (parts.size() < 2)
            continue;
        const QString serial = parts.at(0).trimmed();
        const QString status = parts.at(1).trimmed();
        if (serial.isEmpty() || status.isEmpty())
            continue;
        statuses.insert(serial, status);
    }
    return statuses;
}

void RefreshAdbManager::refreshStatuses(QHash<QString, QString> &statusCache, const QString &adbPath)
{
    const QString cstring = QStringLiteral("\"%1\" devices").arg(adbPath);
    const QString output = getadbOutput(cstring);
    if (!output.contains(QStringLiteral("List of devices"))) {
        logfile(QStringLiteral("Refresh ADB: unexpected adb output, cache unchanged"));
        logfile(output.trimmed().left(200));
        return;
    }
    const QHash<QString, QString> fresh = parseAdbDevices(output);
    for (auto it = statusCache.begin(); it != statusCache.end();) {
        if (!fresh.contains(it.key()))
            it = statusCache.erase(it);
        else
            ++it;
    }
    for (auto it = fresh.constBegin(); it != fresh.constEnd(); ++it)
        statusCache.insert(it.key(), it.value());
    logfile(QStringLiteral("Refresh ADB: %1 device(s) reported").arg(fresh.size()));
}
