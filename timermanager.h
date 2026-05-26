#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include <QObject>
#include <QString>

struct DeviceRecord;

class TimerManager : public QObject
{
    Q_OBJECT

public:
    explicit TimerManager(QObject *parent = nullptr);

    void pushTimers(QWidget *parentWidget, const DeviceRecord &device,
                    const QString &adbPrefix);
};

#endif // TIMERMANAGER_H
