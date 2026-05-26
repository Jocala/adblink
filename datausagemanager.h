#ifndef DATAUSAGEMANAGER_H
#define DATAUSAGEMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

struct DeviceRecord;

class DataUsageManager : public QObject
{
    Q_OBJECT

public:
    using RunLongProcessCallback = std::function<QString(const QString&, const QString&)>;

    explicit DataUsageManager(QObject *parent = nullptr);

    void showKodiDataUsage(QWidget *parentWidget, const DeviceRecord &device,
                           const QString &adbPrefix,
                           RunLongProcessCallback runLongProcess);
};

#endif // DATAUSAGEMANAGER_H
