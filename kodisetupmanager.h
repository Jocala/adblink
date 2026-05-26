#ifndef KODISETUPMANAGER_H
#define KODISETUPMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

struct DeviceRecord;

class KodiSetupManager : public QObject
{
    Q_OBJECT

public:
    using RunLongProcessCallback = std::function<QString(const QString&, const QString&)>;

    explicit KodiSetupManager(QObject *parent = nullptr);

    void createKodiData(QWidget *parentWidget, const DeviceRecord &device,
                        const QString &adbPrefix,
                        RunLongProcessCallback runLongProcess);
};

#endif // KODISETUPMANAGER_H
