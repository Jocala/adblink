#ifndef KEYPADMANAGER_H
#define KEYPADMANAGER_H

#include <QObject>
#include <QString>

struct DeviceRecord;

class KeypadManager : public QObject
{
    Q_OBJECT

public:
    explicit KeypadManager(QObject *parent = nullptr);

    void openKeypad(QWidget *parentWidget, const DeviceRecord &device);
};

#endif // KEYPADMANAGER_H
