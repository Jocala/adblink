#ifndef USBFILEDIALOG_H
#define USBFILEDIALOG_H
#include <QListWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>

#include "customlistwidget.h"

class usbfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit usbfileDialog(bool iskodi, int fmfontsize, QWidget *parent = nullptr);
    ~usbfileDialog();

public:
    void setData(const QString &labelText);
    void setADB(const QString &adbdata);
    void setPath1(QString currentdir);
    void setPath2(QString currentdir);
    void setfmfont(int fmfontsize);
    void setadbdir(QString adbdir);
    void setMode(const int &modenum);
    void setPackagename(const QString &packagename);
    void setkodiPath(const QString &kodipath);
    void editfile(QString fileName, QString xpath);
    void editfile2();
    QString RunProcess_ufd(QString cstring);
    QString RunLongProcess_ufd(QString cstring);
    void do_copy(int opcode);
    void do_xcopy(int opcode);
    void setPulldir(const QString &pullstr);
    void setAdbdir(const QString &adbstr);
    void setuProgram(const QString &fprogram);
    void setdisableroot(const bool &disableroot);
    void setoldfm(const bool &oldfm);
    bool checkRoot();
    QString binfileName();
    QString mcpath;
    void setTitle(QString text);
    QString buffer;
    QString tmpdir1;
    QString tmpstr1;
    QString commstr1;
    QString adb21;
    QString adbdir_ufd;
    QString fmprogram;
    QString cstring;
    QString cstr1;
    QString previous_directory1;
    QString previous_directory2;
    QString pulldir_ufd;
    QString currentitem1;
    QString currentitem2;
    QString current_directory2;
    QString current_directory1;
    QString rootShell;
    QTimer* operationTimer;


    inline static const QString adbShell = QStringLiteral(" shell ");
    inline static const QString quote1 = QStringLiteral("\"'");
    inline static const QString quote2 = QStringLiteral("'\"");

public slots:
    void on_usblistWidget1_doubleClicked(const QModelIndex &index);
    void on_usblistWidget2_doubleClicked(const QModelIndex &index);
    void on_pullButton_clicked();
    void on_copyButton_clicked();
    void on_delButton_clicked();
    void on_editButton_clicked();
    void on_moveButton_clicked();
    void on_resetButton_clicked();
    void on_goButton_clicked();
    void on_clearButton_clicked();
    void on_mkdirButton_clicked();
    void on_renameButton_clicked();
    void on_customdir_returnPressed();
    void assignWindow1();
    void assignWindow2();
    void usbTimerEvent();
    void handleFilesDropped(const QStringList &fileNames, const QString &targetDir);

private:
    void setPathCommon(QListWidget *widget, QString &currentDir, QString &previousDir, QString &currentItem, const QString &dir);
    int    fmfont;
    bool hasfocus = false;
    bool noroot = false;
    QString ufdlogfiledir;
    QString scriptDir;

    CustomListWidget *usblistWidget1 = nullptr;
    CustomListWidget *usblistWidget2 = nullptr;
    QComboBox *kodiDirs = nullptr;
    QProgressBar *usbprogressBar = nullptr;
    QLineEdit *customdir = nullptr;
    QPushButton *goButton = nullptr;


private slots:
    QString fix_directory(QString dirname);
    bool is_directory(QString fdirectory);

    void userpush(QStringList filenames);
     void onExternalFilesDropped(const QStringList &filePaths, const QString &targetDir);

};


#endif // USBFILEDIALOG_H
