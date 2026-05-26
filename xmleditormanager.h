#ifndef XMLEDITORMANAGER_H
#define XMLEDITORMANAGER_H

#include <QObject>
#include "devicerecord.h"

class QWidget;

class XmlEditorManager : public QObject
{
    Q_OBJECT

public:
    explicit XmlEditorManager(QObject *parent = nullptr);
    void editXml(QWidget *parent, const DeviceRecord &device,
                 const QString &adbPrefix, bool scoped,
                 const QString &scriptDir, const QString &busyPath);
};

#endif // XMLEDITORMANAGER_H
