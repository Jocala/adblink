#ifndef KODIDELETEMANAGER_H
#define KODIDELETEMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

class QWidget;
struct DeviceRecord;

class KodiDeleteManager : public QObject
{
    Q_OBJECT

public:
    using RunLongProcessCallback = std::function<QString(const QString&, const QString&)>;

    explicit KodiDeleteManager(QObject *parent = nullptr);

    void deleteKodiData(QWidget *parentWidget, const DeviceRecord &device,
                        const QString &adbPrefix,
                        RunLongProcessCallback runLongProcess);
};

#endif // KODIDELETEMANAGER_H
