#ifndef ADHOCMANAGER_H
#define ADHOCMANAGER_H

#include <QObject>
#include <QString>
#include <functional>

class QLineEdit;

class AdhocManager : public QObject
{
    Q_OBJECT

public:
    using ReloadTableCallback = std::function<void()>;

    explicit AdhocManager(QObject *parent = nullptr);

    void createAdhocRecord(QWidget *parentWidget, QLineEdit *adhocIpEdit,
                           ReloadTableCallback reloadTable);
};

#endif // ADHOCMANAGER_H
