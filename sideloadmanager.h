#ifndef SIDELOADMANAGER_H
#define SIDELOADMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

struct DeviceRecord;

class SideloadManager : public QObject
{
    Q_OBJECT

public:
    using InstallApkCallback = std::function<bool(const QString &filePath)>;
    using ReadInstallCallback = std::function<QString()>;
    using WriteInstallCallback = std::function<void(const QString &dir)>;

    explicit SideloadManager(QObject *parent = nullptr);

    void sideloadApks(QWidget *parentWidget, const DeviceRecord &device,
                      ReadInstallCallback readInstall,
                      InstallApkCallback installApk,
                      WriteInstallCallback writeInstall);
};

#endif // SIDELOADMANAGER_H
