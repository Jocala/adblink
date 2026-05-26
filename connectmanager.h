#ifndef CONNECTMANAGER_H
#define CONNECTMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

struct DeviceRecord;
class QLineEdit;
class QTableWidget;

class ConnectManager : public QObject
{
    Q_OBJECT

public:
    using DeviceQueryCallback = std::function<DeviceRecord(const QString&)>;
    using ValidateIPCallback = std::function<bool(const QString&)>;
    using InfoLogCallback = std::function<void()>;
    using AdhocIpCallback = std::function<void()>;

    explicit ConnectManager(QObject *parent = nullptr);

    void connectToDevice(QWidget *parentWidget,
                         QLineEdit *adhocIpEdit,
                         QTableWidget *deviceTable,
                         DeviceQueryCallback queryDevice,
                         ValidateIPCallback validateIP,
                         InfoLogCallback infoLog,
                         AdhocIpCallback adhocIpHandler,
                         const QString &adbPath);
};

#endif // CONNECTMANAGER_H
