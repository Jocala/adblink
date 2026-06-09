#include "keypadmanager.h"
#include "devicerecord.h"
#include "keyboarddialog.h"

#include <QWidget>

KeypadManager::KeypadManager(QObject *parent)
    : QObject(parent)
{
}

void KeypadManager::openKeypad(QWidget *parentWidget,
                                 const DeviceRecord &device)
{
    QString daddr;
    if (device.isusb) {
        daddr = device.daddr;
    } else {
        QString port = device.port.isEmpty() ? "5555" : device.port;
        daddr = device.daddr + ":" + port;
    }

    keyboardDialog dialog(parentWidget);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setdaddr(daddr);
    dialog.exec();
}
