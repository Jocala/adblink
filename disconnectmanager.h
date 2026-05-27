#ifndef DISCONNECTMANAGER_H
#define DISCONNECTMANAGER_H

#include <QObject>
#include <QString>

class QTableWidget;

class DisconnectManager : public QObject
{
    Q_OBJECT

public:
    explicit DisconnectManager(QObject *parent = nullptr);

    void disconnectDevice(QWidget *parentWidget, QTableWidget *deviceTable);
};

#endif // DISCONNECTMANAGER_H
