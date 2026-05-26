#ifndef KODILOGMANAGER_H
#define KODILOGMANAGER_H

#include <QObject>
#include <QString>

struct DeviceRecord;

class KodiLogManager : public QObject
{
    Q_OBJECT

public:
    explicit KodiLogManager(QObject *parent = nullptr);

    void viewKodiLog(QWidget *parentWidget, const DeviceRecord &device,
                     const QString &adbPrefix, const QString &busyPath);
};

#endif // KODILOGMANAGER_H
