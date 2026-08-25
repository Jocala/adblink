#ifndef ABOUTMANAGER_H
#define ABOUTMANAGER_H

#include <QObject>
#include <QString>

class AboutManager : public QObject
{
    Q_OBJECT

public:
    explicit AboutManager(QObject *parent = nullptr);

    void showAbout(QWidget *parentWidget, const QString &databaseDir,
                   const QString &versionText, const QString &donation = QString());
};

#endif // ABOUTMANAGER_H
