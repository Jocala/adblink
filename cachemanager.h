#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QObject>
#include "devicerecord.h"

class QWidget;

class CacheManager : public QObject
{
    Q_OBJECT

public:
    explicit CacheManager(QObject *parent = nullptr);

    bool configureCache(QWidget *parent, const DeviceRecord &device,
                        const QString &adbPrefix, bool scoped,
                        const QString &scriptDir);
};

#endif // CACHEMANAGER_H
