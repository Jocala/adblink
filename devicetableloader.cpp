#include "devicetableloader.h"
#include "logfile.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSettings>
#include <QSet>
#include <QStringList>
#include <QFont>
#include <QAbstractItemView>
#include <QScrollBar>

DeviceTableLoader::DeviceTableLoader(QObject *parent)
    : QObject(parent)
{
}

void DeviceTableLoader::loadTable(QTableWidget *table, int windowSizeSelector,
                                    int sfont, int mfont, int lfont,
                                    QSize sMain, QSize mMain, QSize lMain,
                                    UsbStatusCallback usbStatus)
{
    QSet<QString> connectedDeviceIds;
    QSet<QString> selectedDescriptions;
    for (int row = 0; row < table->rowCount(); ++row) {
        if (table->item(row, 2) &&
            table->item(row, 2)->text() == "Connected" &&
            table->item(row, 0)) {
            connectedDeviceIds.insert(table->item(row, 0)->data(Qt::UserRole).toString());
        }
        if (table->item(row, 0) && table->item(row, 0)->isSelected())
            selectedDescriptions.insert(table->item(row, 0)->text());
    }

    int savedScroll = table->verticalScrollBar()->value();

    table->clearContents();
    table->setRowCount(0);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(QStringList() << "Device" << "IP" << "Status");
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setShowGrid(true);
    table->setSortingEnabled(false);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    QFont tableFont = table->font();
    switch (windowSizeSelector) {
    case 0: tableFont.setPixelSize(sfont); break;
    case 1: tableFont.setPixelSize(mfont); break;
    case 2: tableFont.setPixelSize(lfont); break;
    default: tableFont.setPixelSize(sfont); break;
    }
    table->setFont(tableFont);

    QString sqlstatement = "SELECT id, description, daddr, isusb FROM device";
    QSqlQuery query;
    if (!query.exec(sqlstatement)) {
        logfile("Query failed: " + query.lastError().text());
        return;
    }

    int row = 0;
    while (query.next()) {
        table->insertRow(row);

        QString deviceId = query.value(0).toString();
        QString description = query.value(1).toString();

        QTableWidgetItem *descItem = new QTableWidgetItem(description);
        descItem->setData(Qt::UserRole, deviceId);
        descItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        table->setItem(row, 0, descItem);

        bool isUsb = query.value(3).toBool();
        QString ip = isUsb ? "USB" : (query.value(2).toString().isEmpty() ? "N/A" : query.value(2).toString());
        table->setItem(row, 1, new QTableWidgetItem(ip));

        QString status;
        if (isUsb)
            status = usbStatus(query.value(2).toString());
        else
            status = connectedDeviceIds.contains(deviceId) ? "Connected" : "Disconnected";

        table->setItem(row, 2, new QTableWidgetItem(status));
        row++;
    }

    int tableWidth;
    switch (windowSizeSelector) {
    case 0: tableWidth = sMain.width() * 0.63; break;
    case 1: tableWidth = mMain.width() * 0.63; break;
    case 2: tableWidth = lMain.width() * 0.55; break;
    default: tableWidth = sMain.width() * 0.63; break;
    }
    int colWidth = tableWidth / 3;
    for (int i = 0; i < 3; ++i) table->setColumnWidth(i, colWidth);

    table->resizeRowsToContents();
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QSettings settings("jocala", "adblink");
    int sortColumn = settings.value("DeviceTableSortColumn", 0).toInt();
    Qt::SortOrder sortOrder = static_cast<Qt::SortOrder>(settings.value("DeviceTableSortOrder", Qt::AscendingOrder).toInt());
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->setSortingEnabled(true);
    table->sortItems(sortColumn, sortOrder);

    for (int r = 0; r < table->rowCount(); ++r) {
        if (table->item(r, 0) && selectedDescriptions.contains(table->item(r, 0)->text()))
            table->selectRow(r);
    }

    table->verticalScrollBar()->setValue(savedScroll);

    table->updateGeometry();
    table->viewport()->update();
    table->update();

    disconnect(table->horizontalHeader(), &QHeaderView::sectionClicked, nullptr, nullptr);
    connect(table->horizontalHeader(), &QHeaderView::sectionClicked, this, [table](int logicalIndex) {
        QSettings settings("jocala", "adblink");
        settings.setValue("DeviceTableSortColumn", logicalIndex);
        settings.setValue("DeviceTableSortOrder", table->horizontalHeader()->sortIndicatorOrder());
    });
}
