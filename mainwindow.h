#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "usbfiledialog.h"

#include <QMainWindow>
#include <QProcess>
#include <QMessageBox>

#include <QTextEdit>
#include <QNetworkAccessManager>
#include <QTableWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QLineEdit>
#include <QScrollArea>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QHeaderView>
#include <QStatusBar>
#include <QProgressBar>
#include <QScrollBar>
#include <QSpacerItem>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSize>
#include "devicerecord.h"

class AdbConnection;
class AdbDevice;
class BackupManager;
class CacheManager;
class ConsoleManager;
class FileManager;
class KodiDataManager;
class KodiDownloader;
class XmlEditorManager;

/////////////////////////////////////////////////////

class IpTableWidgetItem : public QTableWidgetItem {
public:
    IpTableWidgetItem(const QString &text) : QTableWidgetItem(text) {}

    bool operator<(const QTableWidgetItem &other) const override {
        QString thisText = text();
        QString otherText = other.text();

        if (thisText == "N/A") return false;
        if (otherText == "N/A") return true;

        QStringList thisOctets = thisText.split(".");
        QStringList otherOctets = otherText.split(".");

        for (int i = 0; i < 4; ++i) {
            if (i >= thisOctets.size() || i >= otherOctets.size()) {
                return thisOctets.size() < otherOctets.size();
            }
            int thisVal = thisOctets[i].toInt();
            int otherVal = otherOctets[i].toInt();
            if (thisVal != otherVal) {
                return thisVal < otherVal;
            }
        }
        return false;
    }
};

class NoHScrollTableWidget : public QTableWidget {
    Q_OBJECT
public:
    using QTableWidget::QTableWidget;

    NoHScrollTableWidget(QWidget *parent = nullptr) : QTableWidget(parent) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    NoHScrollTableWidget(int rows, int columns, QWidget *parent = nullptr) : QTableWidget(rows, columns, parent) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override {
        int hVal = horizontalScrollBar()->value();
        QTableWidget::scrollTo(index, hint);
        horizontalScrollBar()->setValue(hVal);
    }
};

///////////////////////////////////////

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    DeviceRecord queryDeviceRecord(const QString& description);

private slots:
    bool validateIPAddress(const QString& ipAddress);
    bool renameColumn(const QString& oldColumnName, const QString& newColumnName);
    void onApplicationQuit();
    void serverlabel();
    bool validateDeviceSelection(QString& selectedDescription);
    bool usbConnected(QString daddr);
    void loadDeviceTableX(QTableWidget* table);
    void onReqCompleted();
    void adhocip();
    void on_actionAbout_triggered();
    void connButton_clicked();
    void displayOff();
    void sideload_Button_clicked();
    void setWindowSize();
    void initGridConnections();
    void disButton_clicked();
    void on_actionQuit_triggered();
    void uninstall_Button_clicked();
    bool installAPK(QString filename);
    void on_actionHelp_triggered();
    void backupButton_clicked();
    void restoreButton_clicked();
    void dos_shell();
    void adbshellButton_clicked();
    void delay(int secs);
    void createTables();
    QString getadb();
    QString manufacturer();
    QString devicename();
    QString devicerelease();
    int getandroid();
    bool isScoped();
    void writeBackup(QString dir);
    QString readBackup(QString databasedir);
    QString checkslash(QString qpath);
    void writeInstall(QString dir);
    QString readInstall(QString databasedir);
    void TimerEvent();
    void delayTimer(int rdelay);
    QString RunLongProcess(QString cstring, QString jobname);
    QString RunLongProcess2(QString cstring, QString jobname);

    QString readDonationValue();
    void usbbuttons(bool isusb);
    void rotate_logfile();
    void logfile2(QString line);
    void rebootDevice(QString reboot);
    bool mount_system(QString mnt);
    bool fileExists(QString path);
    bool is_su();
    bool is_busybox();
    QString strip(QString str);
    void kill_server();
    bool start_server();
    bool is_package(QString package);
    void androidLog();
    void on_actionReboot_triggered();
    void deleteRecord(QString descrip);
    bool isConnectedToNetwork();
    void on_donate_clicked();
    void do_versioncheck();
    void on_actionDownload_Kodi_triggered();
    void dataentry(bool isNewRecord);
    void doConsole_clicked();
    void killServer_clicked();
    void screenCap();
    void delRecordButton_clicked();
    void cacheButton_clicked();
    void mvdataButton_clicked();
    void on_actionView_Kodi_Log_triggered();
    void on_actionView_adbLink_Log_triggered();
    void stopapp_clicked();

    void on_actionPaste_path_triggered();
    void on_actionStop_Application_triggered();
    void on_actionStart_Application_triggered();
    void keypadButton_clicked();
    void fmButton_clicked();
    void on_actionMount_system_RO_triggered();
    void on_actionMount_system_RW_triggered();
    void on_actionWireless_ADBD_triggered();
    void on_actionKodi_data_usage_triggered();
    void on_actionKodi_version();

    void on_clearAdhocButton_clicked();
    void on_actionPreferences_triggered();
    void infoArchitecture();
    void infolog();
    void on_actiondelthumb_triggered();
    void on_actionPush_remote_triggered();
    void on_actionCreate_kodi_data_triggered();
    void scpyButton_clicked();
    void pushTimers_clicked();
    void on_actionView_Changelog_triggered();
    void startapp_clicked();
    void on_actionSplash_Screen_triggered();
    void on_View_Changelog_triggered();
    void on_Erase_adbLink_database_triggered();
    void on_actionGet_UID_from_APK_file_triggered();
    void on_actionOculus_VR_triggered();
    void on_actionSend_text_triggered();
    void setupUI();
    void on_actionEdit_XML_triggered();
    void on_actionScreen_Capture_triggered();
    void on_actionKeypad_triggered();
    void on_actionSwitch_View_triggered();
    void switchSize();
    void on_actionReload_devices_triggered();
    void on_infoArchitecture_triggered();
    void setupMenus();

private:
    QNetworkAccessManager *m_networkManager;
    QStringList RunProcessList;
    QPushButton* setupDonateButton(QWidget* parent);
    QPushButton* donateButton;
    void setDonateButtonActive(bool active);

    AdbConnection *m_adbConnection;
    KodiDataManager *m_dataManager;
    ConsoleManager *m_consoleManager;
    BackupManager *m_backupManager;
    CacheManager *m_cacheManager;
    FileManager *m_fileManager;
    XmlEditorManager *m_xmlEditor;
    bool m_isBusy = false;
    KodiDownloader *m_kodiDownloader;

    const QString vqurl = "http://www.jocala.com/version.txt";
    const QString busypath = "/data/local/tmp/adblink/";
    const QString tempdir = "/data/local/tmp/";


    int windowSizeSelector;
    QString apphome = "";
    QString scriptdir = "";
    QString aapt = "";
    QString xmldir = "";
    QString splashdir = "";
    QString adbfiles;
    QString logfiledir;
    QString databasedir;
    QString downloaddir;
    QString scrcpydir;
    QString dbstring = "";
    QString jsonstring = "";
    int currentStack;

    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QWidget *topWidget;
    QHBoxLayout *upperLayout;
    NoHScrollTableWidget *deviceTable;
    QScrollArea *scrollArea;
    QSpacerItem *cosmeticGap;
    QWidget *rightColumnWidget;
    QVBoxLayout *rightLayout;
    QWidget *buttonGridWidget;
    QGridLayout *buttonGridLayout;
    QPushButton *buttons[6];
    QSpacerItem *vSpacer;
    QLineEdit *adhoc_ip;
    QStackedWidget *stackedWidget;
    QWidget *gridWidget1;
    QGridLayout *gridLayout1;
    QPushButton *grid1Buttons[16];
    QWidget *gridWidget2;
    QGridLayout *gridLayout2;
    QPushButton *grid2Buttons[16];
    QWidget *statusBarWidget;
    QVBoxLayout *statusLayout;
    QLabel *statusLabel;
    QProgressBar *progressBar;
    QLabel *server_running;
    QWidget *container;

    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuKodi;
    QMenu *menuUtility;
    QMenu *menuAbout;
    QMenu *menuHelp;

    QAction *actionAbout;
    QAction *actionQuit;
    QAction *actionHelp;
    QAction *actionReboot;
    QAction *actionDownload_Kodi;
    QAction *actionView_Kodi_Log;
    QAction *actionView_adbLink_Log;
    QAction *actionWireless_ADBD;
    QAction *actionKodi_data_usage;
    QAction *actionKodi_version;
    QAction *actionPreferences;
    QAction *infoArchitecture2;
    QAction *actiondelthumb;
    QAction *actionPush_remote;
    QAction *actionSplash_Screen;
    QAction *View_Changelog;
    QAction *Erase_adbLink_database;
    QAction *actionReiinstall_Busybox;
    QAction *actionSet_Kodi_permissions;
    QAction *actionGet_UID_from_APK_file;
    QAction *actionSend_text;
    QAction *actionCreate_kodi_data;
    QAction *actionEdit_XML;
    QAction *actionSwitch_View;
    QAction *actionReload_devices;
    QAction *actionOculus;
    QAction *actionSize;

    const QSize lMainWindowSize = QSize(850, 525);
    const QSize lGridButtonSize = QSize(180, 66);
    const QSize l6ButtonSize = QSize(160, 32);

    const QSize mMainWindowSize = QSize(750, 425);
    const QSize mGridButtonSize = QSize(130, 46);
    const QSize m6ButtonSize = QSize(110, 32);

    const QSize sMainWindowSize = QSize(710, 375);
    const QSize sGridButtonSize = QSize(110, 36);
    const QSize s6ButtonSize = QSize(100, 32);

    int lfontsize = 22;
    int mfontsize = 20;
    int sfontsize = 18;
    int ffontsize = 12;

    QSize buttonsize;
    QSize windowsize;
    QSize ebuttonsize;
};

#endif // MAINWINDOW_H
