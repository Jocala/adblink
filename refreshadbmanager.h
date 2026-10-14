#ifndef REFRESHADBMANAGER_H
#define REFRESHADBMANAGER_H

#include <QHash>
#include <QObject>
#include <QString>

class RefreshAdbManager : public QObject
{
    Q_OBJECT

public:
    explicit RefreshAdbManager(QObject *parent = nullptr);

    static QHash<QString, QString> parseAdbDevices(const QString &output);

    void refreshStatuses(QHash<QString, QString> &statusCache, const QString &adbPath);
};

#endif // REFRESHADBMANAGER_H
