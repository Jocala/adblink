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
    QString port = device.port.isEmpty() ? "5555" : device.port;
    QString daddr = device.daddr + ":" + port;

    QString cstring = daddr + " shell input keyevent ";
    keyboardDialog dialog(parentWidget);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setdaddr(cstring);
    dialog.exec();
}
