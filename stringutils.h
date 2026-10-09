#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QHash>
#include <QString>

QString stripString(const QString &str);
QString checkslash(const QString &qpath);
bool validateIPAddress(const QString &ipAddress);
// True for a syntactically valid Android package name, e.g. "org.xbmc.kodi".
bool isValidPackageName(const QString &package);
// True if "pm list packages" output lists exactly `package`.
bool packageListHas(const QString &pmOutput, const QString &package);
QString usbStatus(const QHash<QString, QString> &cache, const QString &daddr);

#endif // STRINGUTILS_H
