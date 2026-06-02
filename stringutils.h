#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QHash>
#include <QString>

QString stripString(const QString &str);
QString checkslash(const QString &qpath);
bool validateIPAddress(const QString &ipAddress);
QString usbStatus(const QHash<QString, QString> &cache, const QString &daddr);

#endif // STRINGUTILS_H
