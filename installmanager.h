#ifndef INSTALLMANAGER_H
#define INSTALLMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

class InstallManager : public QObject
{
    Q_OBJECT

public:
    using RunLongProcessCallback = std::function<QString(const QString&, const QString&)>;

    explicit InstallManager(QObject *parent = nullptr);

    bool installApk(QWidget *parentWidget, const QString &adbPrefix,
                    const QString &filename, RunLongProcessCallback runLongProcess);

private:
    int m_activeInstalls = 0;
};

#endif // INSTALLMANAGER_H
