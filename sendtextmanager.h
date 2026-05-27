#ifndef SENDTEXTMANAGER_H
#define SENDTEXTMANAGER_H

#include <QObject>
#include <QString>

struct DeviceRecord;

class SendTextManager : public QObject
{
    Q_OBJECT

public:
    explicit SendTextManager(QObject *parent = nullptr);

    void sendText(QWidget *parentWidget, const DeviceRecord &device,
                  const QString &adbPrefix);
};

#endif // SENDTEXTMANAGER_H
