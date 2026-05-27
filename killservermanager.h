#ifndef KILLSERVERMANAGER_H
#define KILLSERVERMANAGER_H

#include <QObject>
#include <QString>

class QTableWidget;

class KillServerManager : public QObject
{
    Q_OBJECT

public:
    explicit KillServerManager(QObject *parent = nullptr);

    void killServer(QWidget *parentWidget, QTableWidget *deviceTable);
};

#endif // KILLSERVERMANAGER_H
