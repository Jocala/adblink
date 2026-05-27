#ifndef THUMBNAILMANAGER_H
#define THUMBNAILMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

struct DeviceRecord;

class ThumbnailManager : public QObject
{
    Q_OBJECT

public:
    using RunLongProcessCallback = std::function<QString(const QString&, const QString&)>;

    explicit ThumbnailManager(QObject *parent = nullptr);

    void deleteThumbnails(QWidget *parentWidget, const DeviceRecord &device,
                          const QString &adbPrefix,
                          RunLongProcessCallback runLongProcess);
};

#endif // THUMBNAILMANAGER_H
