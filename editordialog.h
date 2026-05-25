#ifndef EDITORDIALOG_H
#define EDITORDIALOG_H

#include <QDialog>

class QLabel;
class QTextBrowser;
class QPushButton;
class QClipboard;

class editorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit editorDialog(QWidget *parent = nullptr);
    ~editorDialog();

    void seteditor(const QString &xmlfile);
    void setfilename(const QString &filename);
    QString xmlfile();

private slots:
    void on_copyButton_clicked();
    void on_searchButton_clicked();

private:
    QLabel *m_editorfilename;
    QTextBrowser *m_editorBrowser;
    QPushButton *m_editquitButton;
    QPushButton *m_copyButton;
    QPushButton *m_searchButton;
    QPushButton *m_saveButton;
};

extern int edos;
extern QClipboard *edclipboard;

#endif // EDITORDIALOG_H
