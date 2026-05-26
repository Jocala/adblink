#ifndef DATAMOVEMANAGER_H
#define DATAMOVEMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

struct DeviceRecord;

class DataMoveManager : public QObject
{
    Q_OBJECT

public:
    using RunLongProcessCallback = std::function<QString(const QString&, const QString&)>;

    explicit DataMoveManager(QObject *parent = nullptr);

    void moveKodiData(QWidget *parentWidget, const DeviceRecord &device,
                      const QString &adbPrefix, bool scoped,
                      RunLongProcessCallback runLongProcess);
};

#endif // DATAMOVEMANAGER_H
