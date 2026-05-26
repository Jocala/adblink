#ifndef UNINSTALLMANAGER_H
#define UNINSTALLMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

struct DeviceRecord;

class UninstallManager : public QObject
{
    Q_OBJECT

public:
    using RunLongProcessCallback = std::function<QString(const QString&, const QString&)>;

    explicit UninstallManager(QObject *parent = nullptr);

    void uninstallPackage(QWidget *parentWidget, const DeviceRecord &device,
                          const QString &adbPrefix,
                          RunLongProcessCallback runLongProcess);
};

#endif // UNINSTALLMANAGER_H
