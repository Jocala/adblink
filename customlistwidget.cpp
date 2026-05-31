#include "customlistwidget.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QDataStream>
#include "usbfiledialog.h"

CustomListWidget::CustomListWidget(QWidget *parent) : QListWidget(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setSelectionMode(QAbstractItemView::MultiSelection);
    //         << "acceptDrops:" << acceptDrops() << "dragDropMode:" << dragDropMode()
    //         << "selectionMode:" << selectionMode();

    createContextMenu();
}

void CustomListWidget::createContextMenu()
{

    contextMenu = new QMenu(this);
    copyAction = new QAction("Copy", this);
    moveAction = new QAction("Move", this);
    renameAction = new QAction("Rename", this);
    deleteAction = new QAction("Delete", this);
    editAction = new QAction("Edit text", this);
    newAction = new QAction("New text", this);
    mkdirAction = new QAction("Mkdir", this);
    pullAction = new QAction("Pull", this);
    homeAction = new QAction("Home", this);
    exitAction = new QAction("Close", this);

    contextMenu->addAction(copyAction);
    contextMenu->addAction(moveAction);
    contextMenu->addAction(renameAction);
    contextMenu->addAction(deleteAction);
    contextMenu->addAction(mkdirAction);
    contextMenu->addAction(pullAction);
     contextMenu->addSeparator();
    contextMenu->addAction(editAction);
    contextMenu->addAction(newAction);

    contextMenu->addSeparator();

    contextMenu->addAction(homeAction);
    contextMenu->addAction(exitAction);

    // Connect actions to usbfileDialog slots
    usbfileDialog *parentDialog = qobject_cast<usbfileDialog*>(parent());
    if (parentDialog) {
        connect(copyAction, &QAction::triggered, parentDialog, &usbfileDialog::on_copyButton_clicked);
        connect(moveAction, &QAction::triggered, parentDialog, &usbfileDialog::on_moveButton_clicked);
        connect(renameAction, &QAction::triggered, parentDialog, &usbfileDialog::on_renameButton_clicked);
        connect(deleteAction, &QAction::triggered, parentDialog, &usbfileDialog::on_delButton_clicked);
        connect(editAction, &QAction::triggered, parentDialog, &usbfileDialog::on_editButton_clicked);
        connect(newAction, &QAction::triggered, parentDialog, &usbfileDialog::editfile2);
        connect(mkdirAction, &QAction::triggered, parentDialog, &usbfileDialog::on_mkdirButton_clicked);
        connect(pullAction, &QAction::triggered, parentDialog, &usbfileDialog::on_pullButton_clicked);
        connect(homeAction, &QAction::triggered, parentDialog, &usbfileDialog::on_resetButton_clicked);
        connect(exitAction, &QAction::triggered, parentDialog, &QDialog::close);

    } else {
    }
}

void CustomListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    emit focusRequested(); // Emit signal to set hasfocus

    bool singleSelection = selectedItems().count() == 1;
    renameAction->setEnabled(singleSelection);
    editAction->setEnabled(singleSelection);
    copyAction->setEnabled(!selectedItems().isEmpty());
    moveAction->setEnabled(!selectedItems().isEmpty());
    deleteAction->setEnabled(!selectedItems().isEmpty());
    mkdirAction->setEnabled(true); // Always enabled
    pullAction->setEnabled(!selectedItems().isEmpty()); // Enabled only with selection
    homeAction->setEnabled(true); // Always enabled

    contextMenu->exec(event->globalPos());
}

void CustomListWidget::mousePressEvent(QMouseEvent *event)
{
    emit focusRequested();
    QListWidgetItem *item = itemAt(event->pos());
    if (item) {
        //         << "dragEnabled:" << (item->flags() & Qt::ItemIsDragEnabled);
    } else {
    }
    QListWidget::mousePressEvent(event);
}



void CustomListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") ||
        event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void CustomListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist") ||
        event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void CustomListWidget::dropEvent(QDropEvent *event)
{
    QStringList fileNames;
    QStringList filePaths; // For external files
    QString targetDir = property("currentDirectory").toString();

    // Handle internal drag-and-drop (existing logic)
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        QByteArray modelData = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&modelData, QIODevice::ReadOnly);
        QSet<int> processedRows;
        while (!stream.atEnd()) {
            int row, col;
            QMap<int, QVariant> roleDataMap;
            stream >> row >> col >> roleDataMap;
            if (roleDataMap.contains(Qt::DisplayRole) && !processedRows.contains(row)) {
                QString fileName = roleDataMap[Qt::DisplayRole].toString();
                if (fileName != "..") {
                    fileNames << fileName;
                    processedRows.insert(row);
                }
            }
        }
    }
    // Handle external file drops (e.g., from desktop)
    else if (event->mimeData()->hasFormat("text/uri-list")) {
        QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                filePaths << url.toLocalFile();
            }
        }
    }

    // Process internal drag-and-drop
    if (!fileNames.isEmpty() && !targetDir.isEmpty()) {
        emit filesDropped(fileNames, targetDir);
        event->acceptProposedAction();
    }
    // Process external file drops
    else if (!filePaths.isEmpty() && !targetDir.isEmpty()) {
        emit externalFilesDropped(filePaths, targetDir); // New signal for external files
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
