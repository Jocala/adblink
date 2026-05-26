#ifndef SPLASHSCREENMANAGER_H
#define SPLASHSCREENMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

struct DeviceRecord;

class SplashScreenManager : public QObject
{
    Q_OBJECT

public:
    using RunLongProcessCallback = std::function<QString(const QString&, const QString&)>;

    explicit SplashScreenManager(QObject *parent = nullptr);

    void installSplashScreen(QWidget *parentWidget, const DeviceRecord &device,
                             const QString &adbPrefix,
                             RunLongProcessCallback runLongProcess);
};

#endif // SPLASHSCREENMANAGER_H
