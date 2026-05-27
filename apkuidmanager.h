#ifndef APKUIDMANAGER_H
#define APKUIDMANAGER_H

#include <QObject>
#include <QString>

class ApkUidManager : public QObject
{
    Q_OBJECT

public:
    explicit ApkUidManager(QObject *parent = nullptr);

    void getApkPackageName(QWidget *parentWidget, const QString &aaptPath);
};

#endif // APKUIDMANAGER_H
