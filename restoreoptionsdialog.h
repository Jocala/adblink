#ifndef RESTOREOPTIONSDIALOG_H
#define RESTOREOPTIONSDIALOG_H

#include <QDialog>
#include <QStringList>

class QCheckBox;

class RestoreOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    RestoreOptionsDialog(const QString &deviceName, const QStringList &selected,
                         QWidget *parent = nullptr);

    QStringList checkedFiles() const;

private slots:
    void selectAll();
    void clearAll();

private:
    QStringList m_fileNames;
    QCheckBox *m_fileChecks[9] = {};
};

#endif // RESTOREOPTIONSDIALOG_H
