#ifndef DEVICETABLELOADER_H
#define DEVICETABLELOADER_H

#include <QObject>
#include <QSize>
#include <QString>
#include <functional>

class QTableWidget;

class DeviceTableLoader : public QObject
{
    Q_OBJECT

public:
    using UsbStatusCallback = std::function<QString(const QString &)>;

    explicit DeviceTableLoader(QObject *parent = nullptr);

    void loadTable(QTableWidget *table, int windowSizeSelector,
                   int sfont, int mfont, int lfont,
                   QSize sMain, QSize mMain, QSize lMain,
                   UsbStatusCallback usbStatus);
};

#endif // DEVICETABLELOADER_H
