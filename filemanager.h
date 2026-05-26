#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include "devicerecord.h"

class QWidget;
class usbfileDialog;

class FileManager : public QObject
{
    Q_OBJECT

public:
    struct Context {
        QString adbPrefix;
        QString apphome;
        int fmfont = 12;
        bool isKodiTab = false;
        QString downloadDir;
    };

    explicit FileManager(QObject *parent = nullptr);
    void openFileManager(QWidget *parent, const DeviceRecord &device, const Context &ctx);

private slots:
    void handleFinished();

private:
    usbfileDialog *m_dialog = nullptr;
};

#endif // FILEMANAGER_H
