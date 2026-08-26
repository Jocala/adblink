#include "apkdropdialog.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QFileDialog>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>

ApkDropDialog::ApkDropDialog(QWidget *parent, const QString &initialDir,
                               const QString &title,
                               const QString &actionText,
                               const QString &dropText)
    : QDialog(parent)
    , m_initialDir(initialDir)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(title);
    setFixedSize(520, 380);
    setAcceptDrops(true);
    setWindowModality(Qt::WindowModal);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    QString labelText = dropText.isEmpty() ? tr("Drop APK files here") : dropText;
    m_dropLabel = new QLabel(labelText, this);
    m_dropLabel->setAlignment(Qt::AlignCenter);
    m_dropLabel->setStyleSheet(
        "QLabel {"
        "  border: 2px dashed #8f8f91;"
        "  border-radius: 6px;"
        "  padding: 18px;"
        "  color: #555;"
        "  font-size: 13px;"
        "}"
    );
    m_dropLabel->setFixedHeight(64);
    mainLayout->addWidget(m_dropLabel);

    m_list = new QListWidget(this);
    m_list->setObjectName(QStringLiteral("apkDropList"));
    m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_list->setAcceptDrops(false);
    m_list->setToolTip(tr("Files will be installed in listed order"));
    mainLayout->addWidget(m_list, 1);
    m_list->installEventFilter(this);
    m_dropLabel->installEventFilter(this);

    auto *midButtons = new QHBoxLayout();
    m_browseButton = new QPushButton(tr("Browse..."), this);
    m_removeButton = new QPushButton(tr("Remove"), this);
    m_removeAllButton = new QPushButton(tr("Remove All"), this);
    midButtons->addWidget(m_browseButton);
    midButtons->addWidget(m_removeButton);
    midButtons->addWidget(m_removeAllButton);
    midButtons->addStretch();
    mainLayout->addLayout(midButtons);

    auto *bottomButtons = new QHBoxLayout();
    bottomButtons->addStretch();
    m_cancelButton = new QPushButton(tr("Cancel"), this);
    m_installButton = new QPushButton(actionText.isEmpty() ? tr("Install") : actionText, this);
    m_installButton->setDefault(true);
    bottomButtons->addWidget(m_cancelButton);
    bottomButtons->addWidget(m_installButton);
    mainLayout->addLayout(bottomButtons);

    connect(m_browseButton, &QPushButton::clicked, this, &ApkDropDialog::onBrowse);
    connect(m_removeButton, &QPushButton::clicked, this, &ApkDropDialog::onRemoveSelected);
    connect(m_removeAllButton, &QPushButton::clicked, this, &ApkDropDialog::onRemoveAll);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_installButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_list, &QListWidget::itemSelectionChanged, this, &ApkDropDialog::updateButtons);

    updateButtons();
}

QStringList ApkDropDialog::selectedFiles() const
{
    QStringList files;
    for (int i = 0; i < m_list->count(); ++i)
        files << m_list->item(i)->text();
    return files;
}

bool ApkDropDialog::hasApkUrls(const QMimeData *mime) const
{
    if (!mime->hasUrls())
        return false;
    for (const QUrl &url : mime->urls()) {
        if (url.isLocalFile() && url.toLocalFile().endsWith(QStringLiteral(".apk"), Qt::CaseInsensitive))
            return true;
    }
    return false;
}

void ApkDropDialog::dragEnterEvent(QDragEnterEvent *event)
{
    if (hasApkUrls(event->mimeData()))
        event->acceptProposedAction();
    else
        event->ignore();
}

void ApkDropDialog::dragMoveEvent(QDragMoveEvent *event)
{
    if (hasApkUrls(event->mimeData()))
        event->acceptProposedAction();
    else
        event->ignore();
}

void ApkDropDialog::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    if (!mime->hasUrls()) {
        event->ignore();
        return;
    }
    QStringList paths;
    for (const QUrl &url : mime->urls()) {
        if (!url.isLocalFile())
            continue;
        QString path = url.toLocalFile();
        if (path.endsWith(QStringLiteral(".apk"), Qt::CaseInsensitive))
            paths << path;
    }
    if (paths.isEmpty()) {
        event->ignore();
        return;
    }
    addFiles(paths);
    event->acceptProposedAction();
}

void ApkDropDialog::addFiles(const QStringList &paths)
{
    for (const QString &p : paths) {
        QFileInfo fi(p);
        if (!fi.isFile())
            continue;
        QString canonical = fi.canonicalFilePath();
        if (canonical.isEmpty())
            canonical = fi.absoluteFilePath();
        bool duplicate = false;
        for (int i = 0; i < m_list->count(); ++i) {
            QFileInfo existing(m_list->item(i)->text());
            QString eCanon = existing.canonicalFilePath();
            if (eCanon.isEmpty())
                eCanon = existing.absoluteFilePath();
            if (eCanon == canonical) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate)
            m_list->addItem(canonical);
    }
    if (!paths.isEmpty() && m_initialDir.isEmpty())
        m_initialDir = QFileInfo(paths.first()).absolutePath();
    updateButtons();
}

void ApkDropDialog::onBrowse()
{
    QString dir = m_initialDir;
    if (dir.isEmpty())
        dir = QDir::homePath();
    QFileDialog dialog(this, tr("Select APK files"), dir, tr("APK files (*.apk);;All files (*.*)"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("APK files (*.apk);;All files (*.*)"));
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    if (dialog.exec())
        addFiles(dialog.selectedFiles());
}

void ApkDropDialog::onRemoveSelected()
{
    auto items = m_list->selectedItems();
    for (auto *it : items)
        delete m_list->takeItem(m_list->row(it));
    updateButtons();
}

void ApkDropDialog::onRemoveAll()
{
    m_list->clear();
    updateButtons();
}

void ApkDropDialog::updateButtons()
{
    bool hasFiles = m_list->count() > 0;
    bool hasSelection = !m_list->selectedItems().isEmpty();
    m_installButton->setEnabled(hasFiles);
    m_removeButton->setEnabled(hasSelection);
    m_removeAllButton->setEnabled(hasFiles);
}

bool ApkDropDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_list || watched == m_dropLabel) {
        if (event->type() == QEvent::DragEnter) {
            auto *e = static_cast<QDragEnterEvent*>(event);
            if (hasApkUrls(e->mimeData())) {
                e->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::DragMove) {
            auto *e = static_cast<QDragMoveEvent*>(event);
            if (hasApkUrls(e->mimeData())) {
                e->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::Drop) {
            auto *e = static_cast<QDropEvent*>(event);
            const QMimeData *mime = e->mimeData();
            if (mime->hasUrls()) {
                QStringList paths;
                for (const QUrl &url : mime->urls()) {
                    if (!url.isLocalFile())
                        continue;
                    QString path = url.toLocalFile();
                    if (path.endsWith(QStringLiteral(".apk"), Qt::CaseInsensitive))
                        paths << path;
                }
                if (!paths.isEmpty()) {
                    addFiles(paths);
                    e->acceptProposedAction();
                    return true;
                }
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}
