#ifndef KODIARCHDIALOG_H
#define KODIARCHDIALOG_H

#include <QDialog>
#include <QString>

class QButtonGroup;

class KodiArchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KodiArchDialog(const QString &version, QWidget *parent = nullptr);
    ~KodiArchDialog() override = default;

    int selectedArch() const;  // 0 = v7a, 1 = v8a, 2 = website
    QString version() const { return m_version; }

private:
    QString m_version;
    QButtonGroup *m_group;
};

#endif // KODIARCHDIALOG_H
