#ifndef REMOTEPUSHMANAGER_H
#define REMOTEPUSHMANAGER_H

#include <QObject>
#include <QString>

struct DeviceRecord;

class RemotePushManager : public QObject
{
    Q_OBJECT

public:
    explicit RemotePushManager(QObject *parent = nullptr);

    void pushRemoteXml(QWidget *parentWidget, const DeviceRecord &device,
                       const QString &adbPrefix);
};

#endif // REMOTEPUSHMANAGER_H
