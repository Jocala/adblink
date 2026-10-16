#ifndef REBOOTMANAGER_H
#define REBOOTMANAGER_H

#include <QObject>
#include <QString>

class QTableWidget;

class RebootManager : public QObject
{
    Q_OBJECT

public:
    explicit RebootManager(QObject *parent = nullptr);

    void rebootDevice(QWidget *parentWidget, QTableWidget *deviceTable,
                      bool isUsb, const QString &adbPrefix,
                      const QString &mode = QString());
};

#endif // REBOOTMANAGER_H
