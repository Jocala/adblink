#ifndef DATABASERESETMANAGER_H
#define DATABASERESETMANAGER_H

#include <QObject>
#include <QString>

class DatabaseResetManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseResetManager(QObject *parent = nullptr);

    void resetDatabase(QWidget *parentWidget, const QString &databaseDir);
};

#endif // DATABASERESETMANAGER_H
