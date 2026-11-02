#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QByteArray>
#include <QHash>
#include <QList>
#include <QPair>
#include <QString>

QString stripString(const QString &str);
QString checkslash(const QString &qpath);
bool validateIPAddress(const QString &ipAddress);
QString usbStatus(const QHash<QString, QString> &cache, const QString &daddr);
bool isConnectedStatus(const QString &status);
QList<QPair<QString, QString>> parseTrackDevices(QByteArray &buffer);
QString quotePosixArg(const QString &token);
QString quoteCmdArg(const QString &token);
QChar cmdArgForbiddenChar(const QString &token);
QString scrcpyCommandLine(const QString &binary, const QString &daddr,
                          const QString &argval, bool windows,
                          QChar *forbidden = nullptr);

#endif // STRINGUTILS_H
