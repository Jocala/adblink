#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QListWidget>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

class preferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit preferencesDialog(QWidget *parent = nullptr, bool showkodi = true);

    void setPackagename(const QString &packagename);
    void setPulldir(const QString &pulldir);
    void setisusb(const bool &isusb);
    void setscope(const bool &scoped);
    void setwsa(const bool &wsa);

    void setconn(const bool &isconn);
    void setversionLabel(const QString &versiontext);
    void setostype(const QString &ostype);
    void setdaddr(const QString &daddr);
    void setdevicelist(const QStringList &dstringlist);
    void setdescription(const QString &description);
    void setfilepath(const QString &filepath);
    void setscrcpy(const QString &scrcpy);
    void setdataroot(const QString &data_root);
    void setport(const QString &port);
    void setadb_pref(const QString &adb_pref);
    void setrecnum(const QString &recnum);
    void setdisableroot(const bool &disableroot);

    ~preferencesDialog();

public:
    QString xbmcpackageName();
    QString pulldir();
    QString daddr();
    QString description();
    QString filepath();
    QString scrcpy();
    QString data_root();
    QString port();
    QString ostype();

    bool isusb();
    bool scoped();
    bool wsa();
    bool disableroot();

private slots:
    void on_pfolderButton_clicked();
    void on_kodiButton_clicked();
    void on_spmcButton_clicked();
    void on_otherButton_clicked();
    void on_isusb_clicked(bool checked);
    void on_listkodirootBox_clicked();
    void disable_ui();
    void enable_ui();
    void on_ostypeBox_currentIndexChanged(int index);
    void on_filepathButton_clicked();
    void on_listDevicesp_doubleClicked(const QModelIndex &index);
    void on_wsa_clicked(bool checked);
    void on_scoped_clicked(bool checked);

private:
    QPushButton *m_pfolderButton;
    QPushButton *m_filepathButton;
    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QLineEdit *m_packagename;
    QLineEdit *m_pulldir;
    QLineEdit *m_daddr;
    QLineEdit *m_description;
    QLineEdit *m_filepath;
    QLineEdit *m_port;
    QLineEdit *m_scrcpy;
    QLineEdit *m_data_root;

    QCheckBox *m_isusb;
    QCheckBox *m_scoped;
    QCheckBox *m_wsa;
    QCheckBox *m_disableroot;

    QRadioButton *m_kodiButton;
    QRadioButton *m_spmcButton;
    QRadioButton *m_otherButton;

    QComboBox *m_ostypeBox;

    QListWidget *m_listDevicesp;
    QListWidget *m_listkodirootBox;

    QLabel *m_versionLabel;
    QLabel *m_kodi2;
    QLabel *m_kodi3;

    QGroupBox *m_mediaBox;
};

#endif // PREFERENCESDIALOG_H
