#ifndef DELETERECORDMANAGER_H
#define DELETERECORDMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

class QTableWidget;

class DeleteRecordManager : public QObject
{
    Q_OBJECT

public:
    using DeleteRecordCallback = std::function<void(const QString &)>;

    explicit DeleteRecordManager(QObject *parent = nullptr);

    void deleteSelectedDevice(QWidget *parentWidget, QTableWidget *deviceTable,
                              DeleteRecordCallback deleteRecord);
};

#endif // DELETERECORDMANAGER_H
