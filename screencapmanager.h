#ifndef SCREENCAPMANAGER_H
#define SCREENCAPMANAGER_H

#include <QObject>
#include <QString>

struct DeviceRecord;

class ScreenCapManager : public QObject
{
    Q_OBJECT

public:
    explicit ScreenCapManager(QObject *parent = nullptr);

    void captureScreenshot(QWidget *parentWidget, const DeviceRecord &device,
                           const QString &adbPrefix, const QString &databaseDir);
};

#endif // SCREENCAPMANAGER_H
