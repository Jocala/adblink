#ifndef APKDROPDIALOG_H
#define APKDROPDIALOG_H

#include <QDialog>
#include <QStringList>

class QMimeData;

class QListWidget;
class QLabel;
class QPushButton;

class ApkDropDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApkDropDialog(QWidget *parent, const QString &initialDir,
                           const QString &title = QStringLiteral("Install APKs"),
                           const QString &actionText = QStringLiteral("Install"),
                           const QString &dropText = QStringLiteral("Drop APK files here"));
    QStringList selectedFiles() const;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onBrowse();
    void onRemoveSelected();
    void onRemoveAll();
    void updateButtons();

private:
    void addFiles(const QStringList &paths);
    bool hasApkUrls(const QMimeData *mime) const;

    QListWidget *m_list;
    QLabel *m_dropLabel;
    QPushButton *m_browseButton;
    QPushButton *m_removeButton;
    QPushButton *m_removeAllButton;
    QPushButton *m_installButton;
    QPushButton *m_cancelButton;
    QString m_initialDir;
};

#endif // APKDROPDIALOG_H
