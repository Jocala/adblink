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

    void disconnectDevice(QWidget *parentWidget, QTableWidget *deviceTable, bool bypassPrompt = false);
};

#endif // DISCONNECTMANAGER_H
