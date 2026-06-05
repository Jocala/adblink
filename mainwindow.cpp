    #include "mainwindow.h"
    #include <QRegularExpression>
    #include "appmanager.h"
    #include "adhocmanager.h"
    #include "apkuidmanager.h"
    #include "about.h"
    #include "aboutmanager.h"
    #include "helpdialog.h"
     #include "connectadb.h"
      #include "connectmanager.h"
      #include "disconnectmanager.h"
      #include "deviceeditormanager.h"
      #include "devicetableloader.h"
      #include "uninstalldialog.h"
    #include "getreturncode.h"
    #include "editordialog.h"
    #include "keyboarddialog.h"
    #include "usbfiledialog.h"
    #include "listfiledialog.h"
    #include "datadialog.h"
    #include "forcequitdialog.h"
    #include "restdialog.h"
    #include "tcpipdialog.h"
    #include "adbprefdialog.h"
    #include "sleepdialog.h"
    #include "oculusdialog.h"
     #include "scpdialog.h"
      #include "screencapmanager.h"
      #include "sendtextmanager.h"
      #include "sideloadmanager.h"
      #include "program.h"
    #include "getadbdata.h"
    #include "logfile.h"
    #include "adbutils.h"
    #include "getlocaladb.h"
    #include "version.h"
#include "adbconnection.h"
#include "adbdevice.h"
#include "backupmanager.h"
#include "cachemanager.h"
#include "consolemanager.h"
#include "datamovemanager.h"
#include "databaseresetmanager.h"
#include "deleterecordmanager.h"
#include "datausagemanager.h"
#include "filemanager.h"
#include "installmanager.h"
#include "splashscreenmanager.h"
#include "thumbnailmanager.h"
#include "timermanager.h"
#include "uninstallmanager.h"
#include "versioncheckmanager.h"
#include "wirelessadbmanager.h"
#include "xmleditormanager.h"
#include "kodidatamanager.h"
#include "kodidownloader.h"
#include "kodiarchdialog.h"
#include "kodidownloadcoordinator.h"
#include "kodilogmanager.h"
#include "keypadmanager.h"
#include "killservermanager.h"
#include "kodisetupmanager.h"
    #include "oculusmanager.h"
    #include "preferencesmanager.h"
#include "stringutils.h"
#include "remotepushmanager.h"
#include "rebootmanager.h"
    #include "deviceeditor.h"

    #ifdef __WIN32__
      #include "windows.h"
    #endif

    #include <QApplication>
    #include <QtConcurrent/QtConcurrent>
    #include <qtconcurrentrun.h>
    #include <QMessageBox>
    #include <QTableWidget>
    #include <QResource>
    #include <QProcess>
    #include <QDir>
    #include <QFileDialog>
    #include <QThread>
    #include <QFile>
    #include <QProgressBar>
    #include <QTimer>
    #include <QLabel>
    #include <QInputDialog>
    #include <QtSql/QSqlDatabase>
    #include <QtSql/QSqlError>
    #include <QtSql/QSqlQuery>
    #include "devicerecorddialog.h"
    #include <QElapsedTimer>
    #include <QTextStream>
    #include <QDate>
    #include <QStringList>
    #include <QListWidget>
    #include <QListWidgetItem>
    #include <QDesktopServices>
    #include <QUrl>
    #include <QPixmap>
    #include <QIcon>
    #include <QObject>
    #include <QNetworkAccessManager>
    #include <QNetworkRequest>
    #include <QNetworkReply>
    #include <QtNetwork/QNetworkInterface>
    #include "logviewerdialog.h"
    #include <QFileInfo>
    #include<QClipboard>
    #include <QShortcut>
    #include <QDebug>
    #include <QStackedWidget>
    #include <QGridLayout>
    #include <QVBoxLayout>
    #include <QHBoxLayout>
    #include <QSettings>
    #include <QScrollArea>
    #include <QScrollBar>
    #include <QtGlobal>
    #include <QHeaderView>
    #include <QStatusBar>
    #include <QMenuBar>
    #include <QMenu>
    #include <QAction>
    #include <QCoreApplication>
    #include <QButtonGroup>
    #include <QRadioButton>
#include <QDialogButtonBox>

     MainWindow::MainWindow(QWidget *parent)
         : QMainWindow(parent)
         , m_networkManager(new QNetworkAccessManager(this))
         , m_adbConnection(new AdbConnection(this))
         , m_aboutManager(new AboutManager(this))
         , m_adhocManager(new AdhocManager(this))
         , m_appManager(new AppManager(this))
         , m_apkUidManager(new ApkUidManager(this))
         , m_dataManager(new KodiDataManager(this))
          , m_consoleManager(new ConsoleManager(this))
           , m_backupManager(new BackupManager(this))
           , m_cacheManager(new CacheManager(this))
           , m_connectManager(new ConnectManager(this))
           , m_disconnectManager(new DisconnectManager(this))
           , m_deviceEditorManager(new DeviceEditorManager(this))
           , m_deviceTableLoader(new DeviceTableLoader(this))
           , m_fileManager(new FileManager(this))
           , m_installManager(new InstallManager(this))
           , m_dataMoveManager(new DataMoveManager(this))
           , m_dataUsageManager(new DataUsageManager(this))
           , m_databaseResetManager(new DatabaseResetManager(this))
           , m_deleteRecordManager(new DeleteRecordManager(this))
           , m_sideloadManager(new SideloadManager(this))
           , m_screenCapManager(new ScreenCapManager(this))
           , m_sendTextManager(new SendTextManager(this))
           , m_rebootManager(new RebootManager(this))
           , m_splashScreenManager(new SplashScreenManager(this))
           , m_thumbnailManager(new ThumbnailManager(this))
           , m_timerManager(new TimerManager(this))
           , m_uninstallManager(new UninstallManager(this))
           , m_versionCheckManager(new VersionCheckManager(this))
           , m_wirelessAdbManager(new WirelessAdbManager(this))
           , m_xmlEditor(new XmlEditorManager(this))
           , m_kodiDownloader(new KodiDownloader(this))
           , m_kodiDownloadCoordinator(new KodiDownloadCoordinator(this))
           , m_kodiLogManager(new KodiLogManager(this))
           , m_kodiSetupManager(new KodiSetupManager(this))
           , m_killServerManager(new KillServerManager(this))
           , m_keypadManager(new KeypadManager(this))
           , m_remotePushManager(new RemotePushManager(this))
      {

#if defined(Q_OS_LINUX)
        m_os = 0;
#elif defined(Q_OS_WIN)
        m_os = 1;
#elif defined(Q_OS_MAC)
        m_os = 2;
#endif


        adbfiles=QCoreApplication::applicationDirPath()+"/adbfiles/";
        aapt=QCoreApplication::applicationDirPath()+"/adbfiles/"+"aapt";
        aapt = '"'+aapt+'"';
        apphome = QCoreApplication::applicationDirPath();
        scrcpydir=QCoreApplication::applicationDirPath()+"/adbfiles/"+"scrcpy/";
        xmldir = adbfiles+"remotes/";
        splashdir = adbfiles+"splash/";

        m_dataManager->os = m_os;
        m_dataManager->logFileDir = logfiledir;
        m_backupManager->setDataManager(m_dataManager);

        connect(m_kodiDownloader, &KodiDownloader::logMessage, this, [](const QString &msg) {
            logfile(msg);
        });


       if (!QFile::exists(getadbpath())) {
           QMessageBox msgBox;
           msgBox.setIcon(QMessageBox::Critical);
           msgBox.setWindowTitle(QString());
           msgBox.setText(QStringLiteral("adb binary missing!\n"));
           msgBox.setStandardButtons(QMessageBox::Cancel);
           msgBox.setWindowModality(Qt::ApplicationModal);
           msgBox.exec();

     }



        if (m_os == 1) {
            databasedir = QDir::homePath() + "/AppData/Roaming/.jocala/";
        } else  {
            databasedir = QDir::homePath() + "/.jocala/";
        }

        scriptdir = databasedir + "scripts/";
        dbstring = databasedir + "adblink.db";
        jsonstring = databasedir + "adblink.json";
        logfiledir = databasedir;

        QDir dir(databasedir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }


        dir = QDir(scriptdir);
        if (!dir.exists()) {
            dir.mkpath(".");

        }




        new QShortcut(QKeySequence("Ctrl+A"), this, SLOT(adbshellButton_clicked()));
        new QShortcut(QKeySequence("Ctrl+B"), this, SLOT(backupButton_clicked()));
        new QShortcut(QKeySequence("Ctrl+C"), this, SLOT(screenCap()));
        new QShortcut(QKeySequence("Ctrl+F"), this, SLOT(fmButton_clicked()));
        new QShortcut(QKeySequence("Ctrl+K"), this, SLOT(keypadButton_clicked()));
        new QShortcut(QKeySequence("Ctrl+N"), this, SLOT(doConsole_clicked()));
        new QShortcut(QKeySequence("Ctrl+O"), this, SLOT(startapp_clicked()));
        new QShortcut(QKeySequence("Ctrl+P"), this, SLOT(stopapp_clicked()));
        new QShortcut(QKeySequence("Ctrl+R"), this, SLOT(restoreButton_clicked()));
        new QShortcut(QKeySequence("Ctrl+S"), this, SLOT(scpyButton_clicked()));
        new QShortcut(QKeySequence("Ctrl+T"), this, SLOT(on_actionSend_text_triggered()));
        new QShortcut(QKeySequence("Ctrl+U"), this, SLOT(uninstall_Button_clicked()));
        new QShortcut(QKeySequence("Ctrl+V"), this, SLOT(sideload_Button_clicked()));
        new QShortcut(QKeySequence("Ctrl+D"), this, SLOT(connButton_clicked()));
        new QShortcut(QKeySequence("Ctrl+E"), this, SLOT(disButton_clicked()));
        new QShortcut(QKeySequence("Ctrl+G"), this, [this]() { dataentry(true); });
        new QShortcut(QKeySequence("Ctrl+W"), this, [this]() { dataentry(false); });
        new QShortcut(QKeySequence("Ctrl+I"), this, SLOT(delRecordButton_clicked()));
        new QShortcut(QKeySequence("Ctrl+J"), this, SLOT(on_clearAdhocButton_clicked()));
        new QShortcut(QKeySequence("Ctrl+X"), this, SLOT(displayOff()));
        new QShortcut(QKeySequence("Ctrl++"), this, SLOT(switchSize()));
        connect(qApp, &QCoreApplication::aboutToQuit, this, &MainWindow::onApplicationQuit);
        connect(&m_progressTimer, &QTimer::timeout, this, &MainWindow::TimerEvent);


        setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);






         setWindowTitle(" ");
          QStatusBar *statusBar = new QStatusBar(this);
          setStatusBar(statusBar);
         progressBar = new QProgressBar(this);
         progressBar->setFormat("");
         server_running = new QLabel("", this);

#ifdef Q_OS_MAC
      //https://bugreports.qt.io/browse/QTBUG-51120
        progressBar->setTextVisible(true);
    #endif //Q_OS_MAC


        donateButton = setupDonateButton(statusBar);

        container = new QWidget(statusBar);
        container->setFixedHeight(statusBar->height());
        container->setMinimumWidth(statusBar->width());


        QHBoxLayout *layout = new QHBoxLayout(container);
        layout->addStretch();
        layout->addWidget(donateButton);
        layout->addStretch();
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        statusBar->addPermanentWidget(container, 1);
        statusBar->addPermanentWidget(server_running);
        statusBar->addPermanentWidget(progressBar);
        progressBar->setHidden(true);

       rotate_logfile();


      QDateTime dateTime = QDateTime::currentDateTime();
      QString dtstr = dateTime.toString("MM/dd/yy hh:mm:ss");


      logfile(program + " " + version);

      if (m_os == 1) {
            logfile("Windows");
      } else if (m_os == 2) {
            logfile("macOS");
      } else if (m_os == 0) {
            logfile("Linux");
      } else {
            logfile("Unknown OS");
      }

      logfile(QCoreApplication::applicationDirPath());
      logfile(dtstr);
      logfile("Home: "+QDir::home().path());




         QList<QHostAddress> list = QNetworkInterface::allAddresses();
         QHostAddress primaryIP;


         for (const QHostAddress& address : list) {
             if (!address.isLoopback() && address.protocol() == QAbstractSocket::IPv4Protocol) {
                 if (address != QHostAddress::LocalHost) {
                     primaryIP = address;
                     logfile("IP:"+primaryIP.toString());
                    break;
                 }
             }
         }



         if(!getlocaladb().isEmpty())
          logfile("External adb: "+getlocaladb());




         logfile("------------------------");







        m_dataManager->dataBaseDir = databasedir;
        m_dataManager->logFileDir = logfiledir;
        m_dataManager->scriptDir = scriptdir;

        dbstring = databasedir + "adblink.db";
        if (!m_dataManager->initializeDatabase(dbstring)) {
            QString errorMsg = QSqlDatabase::database().lastError().text();
            logfile(QString("Error opening database: %1 - %2").arg(dbstring, errorMsg));
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(qApp->tr("Cannot open database"));
            msgBox.setText(QStringLiteral("Failed to open database:\n%1\nError: %2").arg(dbstring, errorMsg));
            msgBox.setStandardButtons(QMessageBox::Cancel);
            msgBox.setWindowModality(Qt::ApplicationModal);
            msgBox.exec();
            return;
        }




            deviceTable = new NoHScrollTableWidget(this);

            connect(deviceTable, &QTableWidget::cellDoubleClicked,
                    this, &MainWindow::onDeviceTableDoubleClicked);

             m_dataManager->createTables();
             m_dataManager->createJsonConfig(jsonstring);
            setupMenus();


             setWindowSize();

             m_trackDebounce = new QTimer(this);
             m_trackDebounce->setSingleShot(true);
             connect(m_trackDebounce, &QTimer::timeout, this, &MainWindow::applyDebouncedRefresh);
             startTrackDevices();

             do_versioncheck();




     }





         MainWindow::~MainWindow()
         {



         }



//////////////////////////////////////////////

    void MainWindow::onApplicationQuit() {

      m_quitting = true;

      if (m_trackDevicesProcess) {
          m_trackDevicesProcess->kill();
          m_trackDevicesProcess->waitForFinished(1000);
      }

      QString cstring = getadbpath() + " kill-server";
     QString command=getadbOutput(cstring);
 //    logfile(command);
  //   logfile("server stopped");


       QDir dir(scriptdir);
       dir.setNameFilters(QStringList());
       dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);
       foreach(const QString& dirFile, dir.entryList()) {
                    QString filePath = dir.absoluteFilePath(dirFile);
                    QFile file(filePath);
                    file.setPermissions(QFile::WriteUser | QFile::ReadUser);
//                    file.remove();
       }
    }



    /////////////////////////////////////////////////////
    void MainWindow::TimerEvent()
    {
      int value = progressBar->value();

      if (value >= 100)
          {
             value = 0;
             progressBar->reset();
         }

      progressBar->setValue(value+1);



    }


///////////////////////////////////////

    //////////////////////////////////////////////
    int MainWindow::getandroid()

    {
        // getprop ro.build.fingerprint

        QString cstring = getadb() +   " shell getprop ro.build.version.release";
        QString command=getadbOutput(cstring);
        return command.toInt();

    }



    //////////////////////////////////////////////
    QString MainWindow::manufacturer()

    {

        QString cstring = getadb() +   " shell getprop ro.product.manufacturer";
        QString manufacturer=getadbOutput(cstring);

        return manufacturer;
    }


    //////////////////////////////////////////////
    QString MainWindow::devicename()

    {

        QString cstring = getadb() +   " shell getprop ro.product.device";
        QString device=getadbOutput(cstring);


        device=device.trimmed();



        return device;
    }


    //////////////////////////////////////////////
    QString MainWindow::devicerelease()

    {

        QString cstring = getadb() +   " shell getprop ro.build.display.id";
        QString device=getadbOutput(cstring);


        device=device.trimmed();



        return device;
    }



    /////////////////////////////////////////////

    bool MainWindow::fileExists(QString path) {
        QFileInfo checkFile(path);
        // check if file exists and if yes: Is it really a file and no directory?
        if (checkFile.exists() && checkFile.isFile()) {
            return true;
        } else {
            return false;
        }
    }

    /////////////////////////////////////////

    //////////////////////////////////////////////
    void MainWindow::rotate_logfile()

    {

     rotateLogFile(logfiledir);

    }


    //////////////////////////////////////////////
    void MainWindow::logfile2(QString line)

    {


    QFile file(logfiledir+"adblink.log");
       if(!file.open(QFile::WriteOnly | QFile::Text | QFile::Append))
          {
           QMessageBox msgBox;
           msgBox.setIcon(QMessageBox::Critical);
           msgBox.setWindowTitle(QString());
           msgBox.setText(QStringLiteral("Can't create logfile!\n"));
           msgBox.setStandardButtons(QMessageBox::Cancel);
           msgBox.setWindowModality(Qt::ApplicationModal);
           msgBox.exec();
           return;
          }

       QTextStream out(&file);
                out  << line << Qt::endl;

     }







    ////////////////////////////////////////////////
     //////////////////////////////////////////
     void MainWindow::kill_server()
     {

      m_adbConnection->killServer();

     }





     //////////////////////////////////



    /////////////////////////////////////

    void MainWindow::delay(int secs)
    {
        QTime dieTime= QTime::currentTime().addSecs(secs);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }




////////////////////////////////////////////

    void MainWindow::do_versioncheck()
    {
        m_versionCheckManager->checkVersion(this, databasedir, vqurl,
            stackedWidget, menuKodi, infoArchitecture2,
            m_networkManager,
            [this]() { onReqCompleted(); },
            currentStack);
    }


    void MainWindow::onReqCompleted()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        if (!reply) {
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Network error: %1").arg(reply->errorString()));
            msgBox.setStandardButtons(QMessageBox::Cancel);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            reply->deleteLater();
            return;
        }

        QByteArray data = reply->readAll();
        QString s1(data);
        s1 = s1.trimmed();

        if (version != s1) {
            QDialog dialog(this);
            QVBoxLayout layout(&dialog);
            QLabel messageLabel("adblink version " + s1 + " is ready. Download?");
            layout.addWidget(&messageLabel);

            QHBoxLayout buttonLayout;
            QPushButton yesButton("Yes");
            QPushButton noButton("No");
            QPushButton changelogButton("Changelog");

            buttonLayout.addWidget(&yesButton);
            buttonLayout.addWidget(&noButton);
            buttonLayout.addWidget(&changelogButton);

            layout.addLayout(&buttonLayout);

            connect(&yesButton, &QPushButton::clicked, [&dialog]() {
                QDesktopServices::openUrl(QUrl("https://www.jocala.com"));
                dialog.close();
            });

            connect(&noButton, &QPushButton::clicked, [&dialog]() {
                dialog.close();
            });

            connect(&changelogButton, &QPushButton::clicked, this, &MainWindow::on_actionView_Changelog_triggered);

            dialog.exec();
        }

        reply->deleteLater();
    }





    //////////////////////////////////////////////////////////////////////
    void MainWindow::on_actionQuit_triggered()
    {

        QCoreApplication::quit();

    }







    ////////////////////////////////////////////////////////////
    void MainWindow::sideload_Button_clicked()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_sideloadManager->sideloadApks(this, device,
            [this]() { return readInstall(databasedir); },
            [this](const QString &filePath) { return m_installManager->installApk(this, getadb(), filePath, [this](const QString &cmd, const QString &title) { return RunLongProcess(cmd, title); }); },
            [this](const QString &dir) { writeInstall(dir); });
    }



    ///////////////////////////////////////////////////////////////////////////
    void MainWindow::uninstall_Button_clicked()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_uninstallManager->uninstallPackage(this, device, getadb(),
            [this](const QString &cstring, const QString &jobname) {
                return RunLongProcess(cstring, jobname);
            });
    }


//////////////////////////////////////////////


    void MainWindow::onDeviceTableDoubleClicked(int row, int column)
    {
        if (!deviceTable->item(row, 0) || !deviceTable->item(row, 2))
            return;

        deviceTable->selectRow(row);

        QString status = deviceTable->item(row, 2)->text();

        if (status == "Connected") {
            disButton_clicked();
        } else if (status == "Disconnected") {
            connButton_clicked();
        }
    }

    void MainWindow::connButton_clicked()
    {
        m_connectManager->connectToDevice(this, adhoc_ip, deviceTable,
            [this](const QString &desc) { return queryDeviceRecord(desc); },
            [this](const QString &ip) { return ::validateIPAddress(ip); },
            [this]() { infolog(); },
            [this]() { adhocip(); },
            getadbpath());
    }



////////////////////////////////////////////

    void MainWindow::adhocip()
    {
        m_adhocManager->createAdhocRecord(this, adhoc_ip,
            [this]() { loadDeviceTableX(deviceTable); });
    }


    ////////////////////////////////////////////////////////////////

    void MainWindow::disButton_clicked()
    {
        m_disconnectManager->disconnectDevice(this, deviceTable);
    }





    /////////////////////////////////////////////////////////////////////////
    void MainWindow::on_actionAbout_triggered()
    {
        m_aboutManager->showAbout(this, databasedir, program + " " + version);
    }


    //////////////////////////////////////////////
    void MainWindow::on_actionHelp_triggered()
    {
        auto *helpdialog = new helpDialog(this);
        helpdialog->setWindowTitle("Help");
        helpdialog->setAttribute(Qt::WA_DeleteOnClose);
        helpdialog->show();
    }



    void MainWindow::on_actionPush_remote_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_remotePushManager->pushRemoteXml(this, device, getadb());
    }







    ////////////////////////////////////////////////////////////////////////////////
    void MainWindow::on_actionReboot_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_rebootManager->rebootDevice(this, deviceTable, device.isusb, getadb());
    }



    /////////////////////////////////////////
    void MainWindow::on_donate_clicked()
    {


       QString link = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GKZMW456H6E5W";
       QDesktopServices::openUrl(QUrl(link));

    }

////////////////////////////////////////

    void MainWindow::killServer_clicked()
    {
        m_killServerManager->killServer(this, deviceTable);
    }



//////////////////

     void MainWindow::dataentry(bool isNewRecord)
     {
         m_deviceEditorManager->openEditor(this, isNewRecord, deviceTable,
             stackedWidget->currentIndex() == 0, version,
             [this](const QString &desc) { return queryDeviceRecord(desc); },
             [this]() { on_Erase_adbLink_database_triggered(); },
             [this]() { loadDeviceTableX(deviceTable); });
     }


    ///////////////////////////////////////////
     void MainWindow::cacheButton_clicked()
     {
         QString selectedDescription;
         if (!validateDeviceSelection(selectedDescription))
             return;

         DeviceRecord device = queryDeviceRecord(selectedDescription);

         m_cacheManager->configureCache(this, device, getadb(), ::isScopedStorage(getadb()), scriptdir);
     }



    //////////////////////////////////////////////////
    void MainWindow::on_actionView_Kodi_Log_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_kodiLogManager->viewKodiLog(this, device, getadb(), busypath);
    }


    ////////////////////////////////////////////////////
    void MainWindow::on_actionView_adbLink_Log_triggered()
    {
        LogViewerDialog dialog(this);
        dialog.loadAdblinkLog();
        dialog.setModal(true);
        dialog.exec();

    }



    ///////////////////////////////////////////////


    //////////////////////////////////////////////////////////
    void MainWindow::on_actionPaste_path_triggered()
    {

        QClipboard *pathClipboard = QApplication::clipboard();
        pathClipboard->setText("PATH=$PATH:/data/local/tmp/adblink\n");
    }


    //////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////

    void MainWindow::on_actionStop_Application_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_appManager->stopApp(this, device, getadb(), databasedir,
                              QStringLiteral("stopapp.json"));
    }


    /////////////////////////////////////////////////////////

    void MainWindow::on_actionStart_Application_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_appManager->startApp(this, device, getadb(), databasedir,
                               QStringLiteral("startapp.json"));
    }


    ////////////////////////////////////////////////

    void MainWindow::keypadButton_clicked()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_keypadManager->openKeypad(this, device);
    }



    ///////////////////////////////////////////////////////


     void MainWindow::fmButton_clicked()
     {
         QString selectedDescription;
         if (!validateDeviceSelection(selectedDescription))
             return;

         DeviceRecord device = queryDeviceRecord(selectedDescription);

         QFile file(databasedir + "adblink.json");
         (void)file.open(QIODevice::ReadOnly);
         QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
         QString download = doc.object()["download"].toString();

         FileManager::Context ctx;
         ctx.adbPrefix = getadb();
         ctx.apphome = apphome;
         ctx.fmfont = ffontsize;
         ctx.isKodiTab = (stackedWidget->currentIndex() == 0);
         ctx.downloadDir = download;

         m_fileManager->openFileManager(this, device, ctx);
     }





    QString MainWindow::RunLongProcess(QString cstring, QString jobname)
    {


        progressBar->setHidden(false);
        progressBar->setValue(0);
        QString command;
        QString s = jobname;
        RunProcessList << s;
        container->setHidden(true);

        if (RunProcessList.count() > 1)
           server_running->setText("parallel processes running");
        else
           server_running->setText(s);

        m_progressTimer.start(4000);

        command = getadbOutput(cstring);
        m_progressTimer.stop();

        RunProcessList.removeAll(s);

        if (RunProcessList.count() > 0)
        {
           //activityIcon(true);
           progressBar->setHidden(false);
           progressBar->setValue(0);
           server_running->setText("");
        }
        else
        {
           //activityIcon(false);
           progressBar->setHidden(true);
            container->setHidden(false);
           progressBar->setValue(0);
            server_running->setText("");
        }

        return command;
    }


   ////////////////////////////////////////////////////////




    void MainWindow::on_actionWireless_ADBD_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_wirelessAdbManager->enableWirelessAdb(this, device, getadb());
    }






    ////////////////////////////////////////////////////////

    void MainWindow::on_actionKodi_data_usage_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_dataUsageManager->showKodiDataUsage(this, device, getadb(),
            [this](const QString &cstring, const QString &jobname) {
                return RunLongProcess(cstring, jobname);
            });
    }





     ////////////////////////////////////////////////////////

     void MainWindow::on_actionKodi_version()
     {
         QString selectedDescription;
         if (!validateDeviceSelection(selectedDescription))
             return;

         DeviceRecord device = queryDeviceRecord(selectedDescription);

         QString installedVer = m_kodiDownloader->installedVersionOnDevice(
             getadb(), device.daddr);
         if (installedVer.isEmpty()) {
             installedVer = "Unknown";
             logfile("Failed to retrieve installed Kodi version.");
         } else {
             logfile("Installed Kodi version: " + installedVer);
         }

         disconnect(m_kodiDownloader, &KodiDownloader::versionFetched, this, nullptr);

         connect(m_kodiDownloader, &KodiDownloader::versionFetched, this,
             [this, installedVer](const QString &latestVer) {

             disconnect(m_kodiDownloader, &KodiDownloader::versionFetched, this, nullptr);

             bool isUpToDate = !KodiDownloader::isNewerVersionAvailable(installedVer, latestVer);

             logfile("Latest Kodi version: " + latestVer);

             QString message;
             if (installedVer == "Unknown" || latestVer == "Unknown") {
                 message = "Cannot compare versions.\n"
                           "Installed Kodi Version: " + installedVer + "\n"
                           "Latest Stable Version: " + latestVer;
             } else {
                 message = "Installed Kodi Version: " + installedVer + "\n"
                           "Latest Stable Version: " + latestVer + "\n"
                         + (isUpToDate ? "Your Kodi version is up to date."
                                       : "A newer version of Kodi is available.");
             }

             logfile(message);

             if (!isUpToDate && installedVer != "Unknown" && latestVer != "Unknown") {
                 QMessageBox msgBox(this);
                 msgBox.setWindowTitle("Kodi Version Check");
                 msgBox.setText(message + "\n\nWould you like to download " + latestVer + "?");
                 QPushButton *downloadBtn = msgBox.addButton("Download", QMessageBox::ActionRole);
                 msgBox.addButton(QMessageBox::Cancel);
                 msgBox.exec();
                 if (msgBox.clickedButton() == downloadBtn)
                     on_actionDownload_Kodi_triggered();
             } else {
                 QMessageBox msgBox(this);
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setWindowTitle(QStringLiteral("Kodi Version Check"));
                msgBox.setText(message);
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setWindowModality(Qt::WindowModal);
                msgBox.exec();
             }
         });

         m_kodiDownloader->fetchLatestVersion();
     }



    ////////////////////////////////////////////////////////////////////////

    void MainWindow::on_clearAdhocButton_clicked()
    {
        adhoc_ip->setText("");


    }


////////////////////////////////////////////////////////////

    void MainWindow::on_actionPreferences_triggered()
    {
         PreferencesManager mgr(databasedir + "adblink.json", m_os);
         if (!mgr.exec(this))
             return;

         PreferencesResult r = mgr.result();

         switch (r.lgFontIndex) {
         case 0: lfontsize = 14; break;
         case 1: lfontsize = 16; break;
         case 2: lfontsize = 18; break;
         case 3: lfontsize = 20; break;
         default: lfontsize = 14; break;
         }
         switch (r.mdFontIndex) {
         case 0: mfontsize = 12; break;
         case 1: mfontsize = 14; break;
         case 2: mfontsize = 16; break;
         case 3: mfontsize = 18; break;
         default: mfontsize = 14; break;
         }
         switch (r.smFontIndex) {
         case 0: sfontsize = 10; break;
         case 1: sfontsize = 12; break;
         case 2: sfontsize = 14; break;
         case 3: sfontsize = 16; break;
         default: sfontsize = 12; break;
         }
         switch (r.fmFontIndex) {
         case 0: ffontsize = 12; break;
         case 1: ffontsize = 16; break;
         case 2: ffontsize = 18; break;
         case 3: ffontsize = 22; break;
         default: ffontsize = 16; break;
         }

          setWindowSize();
     }



    ////////////////////////////////////////////////


    void MainWindow::on_actionDownload_Kodi_triggered()
    {
        m_kodiDownloadCoordinator->downloadKodi(this, databasedir, m_kodiDownloader,
            progressBar, server_running, container,
            [this](const QString &filePath) {
                QString desc;
                if (validateDeviceSelection(desc))
                    m_installManager->installApk(this, getadb(), filePath,
                        [this](const QString &cmd, const QString &title) { return RunLongProcess(cmd, title); });
                return true;
            },
            [this]() { on_actionView_adbLink_Log_triggered(); },
            []() { });
    }



    /////////////////////////////////////////////////////////



    void MainWindow::infoArchitecture()
    {
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
                   return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    QString cstring = getadb() + " shell getprop ro.product.cpu.abi";
    QString archi = getadbOutput(cstring);
    QString android = QString::number(getandroid());
    QString adevice = devicename();
    QString bdevice = devicerelease();
    QString manufact = manufacturer();
    QString scoped = ::isScopedStorage(getadb()) ? "true" : "false";
    QString battinf = ::readBatteryLevel(getadb());



    archi   =  "Architecture:    "+archi.trimmed();
    android =  "Android version: "+android.trimmed();
    adevice =  "Device name:     "+adevice.trimmed();
    bdevice =  "Device release:  "+bdevice.trimmed();
    manufact =  "Manufacturer:   "+manufact.trimmed();
    battinf =  "Battery level:   "+battinf.trimmed();
    scoped =   "Scoped storage:  "+scoped.trimmed();


    logfile(" ");
    logfile("Device: "+selectedDescription);
    logfile("------------------------");
    logfile(archi);
    logfile(android);
    logfile(adevice);
    logfile(bdevice);
    logfile(manufact);
    logfile(battinf);
    logfile(scoped);
    logfile("------------------------");

    QStringList list;
    list << archi << android << adevice << bdevice << manufact << battinf << scoped;



    QDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    dialog.setWindowTitle(device.description);
    dialog.setFixedWidth(350);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);


    for (const QString &item : list) {
                   QLabel *label = new QLabel(item, &dialog);
                   layout->addWidget(label);
    }


    QPushButton *okButton = new QPushButton("OK", &dialog);
    layout->addWidget(okButton, 0, Qt::AlignCenter);


    QObject::connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);


    dialog.setLayout(layout);
    dialog.setModal(true);
    dialog.exec();
    }


    // adb shell dumpsys battery
    // present == false no batt
    //ro.product.product.device
    //ro.product.manufacturer
    // ro.product.brand
    //  adb shell getprop ro.build.version.release



///////////////////////////////////////////////////////
    void MainWindow::infolog()
    {
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
                   return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    QString cstring = getadb() + " shell getprop ro.product.cpu.abi";
    QString archi = getadbOutput(cstring);
    QString android = QString::number(getandroid());
    QString adevice = devicename();
    QString bdevice = devicerelease();
    QString manufact = manufacturer();
    QString scoped = ::isScopedStorage(getadb()) ? "true" : "false";
    QString battinf = ::readBatteryLevel(getadb());



    archi   =  "Architecture:    "+archi.trimmed();
    android =  "Android version: "+android.trimmed();
    adevice =  "Device name:     "+adevice.trimmed();
    bdevice =  "Device release:  "+bdevice.trimmed();
    manufact =  "Manufacturer:   "+manufact.trimmed();
    battinf =  "Battery level:   "+battinf.trimmed();
    scoped =   "Scoped storage:  "+scoped.trimmed();

    logfile(" ");
    logfile("Device: "+selectedDescription);
    logfile("------------------------");
    logfile(archi);
    logfile(android);
    logfile(adevice);
    logfile(bdevice);
    logfile(manufact);
    logfile(battinf);
    logfile(scoped);
    logfile("------------------------");

    }




////////////////////////////////////////////

  void MainWindow::pushTimers_clicked()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_timerManager->pushTimers(this, device, getadb());
    }

//////////////////////////////////////////////////////

    void MainWindow::on_actiondelthumb_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_thumbnailManager->deleteThumbnails(this, device, getadb(),
            [this](const QString &cstring, const QString &jobname) {
                return RunLongProcess(cstring, jobname);
            });
    }





////////////////////////////////////////////////////////////////

    void MainWindow::on_actionCreate_kodi_data_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription))
            return;

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        m_kodiSetupManager->createKodiData(this, device, getadb(),
            [this](const QString &cstring, const QString &jobname) {
                return RunLongProcess(cstring, jobname);
            });
    }











    ////////////////////////////////////////

       void MainWindow::doConsole_clicked()
       {
           m_consoleManager->openConsole(scriptdir, apphome, databasedir);
       }

   ////////////////////////////////////////////////


       void MainWindow::adbshellButton_clicked()
       {
           QString port;
           QString daddr;

           QString selectedDescription;
           if (!validateDeviceSelection(selectedDescription))
               return;

           DeviceRecord device = queryDeviceRecord(selectedDescription);

           if (device.isusb) {
               port = "";
               daddr = device.daddr;
           } else {
               port = device.port.isEmpty() ? "5555" : device.port;
               daddr = device.daddr + ":" + port;
           }

           logfile("adb: " + getadbpath());
           logfile("detaching console process");

           m_consoleManager->openAdbShell(daddr, scriptdir, apphome, databasedir);
       }



        void MainWindow::scpyButton_clicked()
        {
            QString port;
            QString daddr;

            QString selectedDescription;
            if (!validateDeviceSelection(selectedDescription))
                return;

            DeviceRecord device = queryDeviceRecord(selectedDescription);

            if (device.isusb) {
                port = "";
                daddr = device.daddr;
            } else {
                port = device.port.isEmpty() ? "5555" : device.port;
                daddr = device.daddr + ":" + port;
            }

            m_consoleManager->openScrcpy(this, daddr, scriptdir, apphome,
                                         databasedir, adbfiles, scrcpydir,
                                         device.scrcpyarg);
        }



QString MainWindow::readBackup(QString /*databasedir*/) {
  return m_dataManager->readBackupPath(jsonstring);
}








///////////////////////////////////////////////////
void MainWindow::writeBackup (QString dir) {
    m_dataManager->writeBackupPath(jsonstring, dir);
}


///////////////////////////////////////////////////////
QString MainWindow::readInstall(QString /*databasedir*/) {
    return m_dataManager->readInstallPath(jsonstring);
}


/////////////////////////////////////////////////////
void MainWindow::writeInstall (QString install) {
    m_dataManager->writeInstallPath(jsonstring, install);
}

/////////////////////////////////////////////

void MainWindow::backupButton_clicked()
{

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
        return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    if (!::isPackageInstalled(getadb(), device.xbmcpackage)) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("%1 not installed").arg(device.xbmcpackage));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        logfile(device.daddr + ": Error: " + device.xbmcpackage + " not installed");
        return;
    }

    const QString adbPrefix = getadb() + " ";

    m_backupManager->backupDevice(this, device, adbPrefix, ::isScopedStorage(getadb()),
                                  m_dataManager->os, jsonstring, m_dataManager,
                                  [this](const QString &cmd, const QString &title) {
                                      return RunLongProcess(cmd, title);
                                  });

}


/////////////////////////////////////////////

void MainWindow::restoreButton_clicked() {

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
        return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    if (!::isPackageInstalled(getadb(), device.xbmcpackage)) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("%1 not installed").arg(device.xbmcpackage));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        logfile(device.daddr + ": Error: " + device.xbmcpackage + " not installed");
        return;
    }

    const QString adbPrefix = getadb() + " ";

    m_backupManager->restoreDevice(this, device, adbPrefix, ::isScopedStorage(getadb()),
                                   jsonstring, m_dataManager,
                                   [this](const QString &cmd, const QString &title) {
                                       return RunLongProcess(cmd, title);
                                   });

}


//////////////////////////////////////////////
void MainWindow::mvdataButton_clicked()
{
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription))
        return;

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    m_dataMoveManager->moveKodiData(this, device, getadb(), ::isScopedStorage(getadb()),
        [this](const QString &cstring, const QString &jobname) {
            return RunLongProcess(cstring, jobname);
        });
}



//////////////////////////////////////////


void MainWindow::on_actionView_Changelog_triggered()
{
    QString link = "https://jocala.com/changelog.txt";
    QDesktopServices::openUrl(QUrl(link));

}




//////////////////////////////////////////


void MainWindow::on_actionSplash_Screen_triggered()
{
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription))
        return;

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    m_splashScreenManager->installSplashScreen(this, device, getadb(),
        [this](const QString &cstring, const QString &jobname) {
            return RunLongProcess(cstring, jobname);
        });
}


void MainWindow::on_View_Changelog_triggered()
{
            QString link = "https://jocala.com/changelog.txt";
            QDesktopServices::openUrl(QUrl(link));
}







void MainWindow::on_actionGet_UID_from_APK_file_triggered()
{
    m_apkUidManager->getApkPackageName(this, aapt);
}



void MainWindow::on_actionSend_text_triggered()
{
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription))
        return;

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    m_sendTextManager->sendText(this, device, getadb());
}





//////////////////////////////////////////////////////////////
void MainWindow::on_actionOculus_VR_triggered()
{
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription))
        return;

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    OculusManager oculus(getadb(), databasedir, this);
    oculus.exec();
}

void MainWindow::on_actionEdit_XML_triggered()
{
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription))
        return;

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    m_xmlEditor->editXml(this, device, getadb(), ::isScopedStorage(getadb()), scriptdir, busypath);
}




//////////////////////////

void MainWindow::on_actionKeypad_triggered()
{
  keypadButton_clicked();
}

//////////////////////////////////

QString MainWindow::getadb()
{


 QString gadb = "";
 QString port;
 QString editport = "";
 QString selectedDescription;
 int selectedRow = deviceTable->currentRow();


           if (selectedRow >= 0 && deviceTable->item(selectedRow, 0)) {
              selectedDescription = deviceTable->item(selectedRow, 0)->text();
           } else {
               QMessageBox msgBox(this);
               msgBox.setIcon(QMessageBox::Critical);
               msgBox.setWindowTitle(QStringLiteral("Error"));
               msgBox.setText(QStringLiteral("No device selected in table"));
               msgBox.setStandardButtons(QMessageBox::Ok);
               msgBox.setWindowModality(Qt::WindowModal);
               msgBox.exec();
              return "error";
           }

            QString daddr = deviceTable->item(selectedRow, 1)->text();

            DeviceRecord device = queryDeviceRecord(selectedDescription);


            if (device.isusb) {
              port = "";
              daddr = device.daddr;
            } else {
              port = device.port.isEmpty() ? "5555" : device.port;
              daddr = device.daddr + ":" + port;
            }



   //     gadb = getadbpath() + " -s " + daddr;

        gadb = QString("\"%1\" -s %2").arg(getadbpath(), daddr);


 return gadb;
}

///////////////////////////////////////////////////

DeviceRecord MainWindow::queryDeviceRecord(const QString& description) {
    return m_dataManager->queryDeviceRecord(description);
}
///////////////////////////////////////


bool MainWindow::validateDeviceSelection(QString& selectedDescription) {
 // Check if any device is connected in deviceTable
 bool hasConnectedDevice = false;
 for (int i = 0; i < deviceTable->rowCount(); ++i) {
               if (deviceTable->item(i, 2) &&
                   deviceTable->item(i, 2)->text() == "Connected") {
              hasConnectedDevice = true;
              break;
               }
 }
 if (!hasConnectedDevice) {
               QMessageBox msgBox(this);
               msgBox.setIcon(QMessageBox::Critical);
               msgBox.setWindowTitle(QString());
               msgBox.setText(QStringLiteral("No devices connected"));
               msgBox.setStandardButtons(QMessageBox::Ok);
               msgBox.setWindowModality(Qt::WindowModal);
               msgBox.exec();
               return false;
 }

 // Get selected description from deviceTable
 int selectedRow = deviceTable->currentRow();
 if (selectedRow >= 0 && deviceTable->item(selectedRow, 0)) {
               selectedDescription = deviceTable->item(selectedRow, 0)->text();
 } else {
               QMessageBox msgBox(this);
               msgBox.setIcon(QMessageBox::Critical);
               msgBox.setWindowTitle(QString());
               msgBox.setText(QStringLiteral("No device selected in table"));
               msgBox.setStandardButtons(QMessageBox::Ok);
               msgBox.setWindowModality(Qt::WindowModal);
               msgBox.exec();
               return false;
 }

 // Check if the selected device is connected
 if (deviceTable->item(selectedRow, 2) &&
     deviceTable->item(selectedRow, 2)->text() != "Connected") {
               QMessageBox msgBox(this);
               msgBox.setIcon(QMessageBox::Critical);
               msgBox.setWindowTitle(QString());
               msgBox.setText(QStringLiteral("Selected device is not connected"));
               msgBox.setStandardButtons(QMessageBox::Ok);
               msgBox.setWindowModality(Qt::WindowModal);
               msgBox.exec();
               return false;
 }

 return true;
}








void MainWindow::screenCap()
{
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription))
        return;

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    m_screenCapManager->captureScreenshot(this, device, getadb(), databasedir);
}



//////////////////////////////////////


////////////////////////////////////////////////////////////

////////////////////////////////////////

void MainWindow::delRecordButton_clicked()
{
    m_deleteRecordManager->deleteSelectedDevice(this, deviceTable,
        [this](const QString &descrip) { deleteRecord(descrip); });
}



///////////////////////////////////////////////////


void MainWindow::createTables()
{
    m_dataManager->createTables();
}



void MainWindow::deleteRecord(QString descrip)
{
    m_dataManager->deleteRecord(descrip);
}

void MainWindow::on_Erase_adbLink_database_triggered()
{
    m_databaseResetManager->resetDatabase(this, databasedir);
}



//////////////////////////////////////////

void MainWindow::stopapp_clicked()
{
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription))
        return;

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    m_appManager->stopApp(this, device, getadb(), databasedir);
}




/////////////////////////////////////////

void MainWindow::startapp_clicked()
{
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription))
        return;

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    m_appManager->startApp(this, device, getadb(), databasedir);
}



////////////////////////////////

////////////////////////////////////

void MainWindow::displayOff()
{
    ::displayOff(getadb());
}




void MainWindow::startTrackDevices()
{
    if (m_trackDevicesProcess) {
        m_trackDevicesProcess->kill();
        m_trackDevicesProcess->deleteLater();
        m_trackDevicesProcess = nullptr;
    }

    m_trackDevicesProcess = new QProcess(this);
    connect(m_trackDevicesProcess, &QProcess::readyReadStandardOutput,
            this, &MainWindow::onTrackOutput);
    connect(m_trackDevicesProcess, &QProcess::finished,
            this, &MainWindow::onTrackFinished);

    m_trackDevicesProcess->start(getadbpath(), QStringList() << QStringLiteral("track-devices"));
}

void MainWindow::onTrackOutput()
{
    while (m_trackDevicesProcess->canReadLine()) {
        QByteArray line = m_trackDevicesProcess->readLine().trimmed();
        if (line.isEmpty())
            continue;

        QString lineStr = QString::fromLatin1(line);
        QString serial = lineStr.section('\t', 0, 0).trimmed();
        int firstLetter = 0;
        while (firstLetter < serial.length() && serial.at(firstLetter).isDigit())
            firstLetter++;
        if (firstLetter > 0)
            serial = serial.mid(firstLetter);
        QString status = lineStr.section('\t', 1, 1).trimmed();
        if (serial.isEmpty())
            continue;

        if (status == QLatin1String("offline"))
            m_usbStatusCache.remove(serial);
        else
            m_usbStatusCache[serial] = status;
    }

    m_trackDebounce->start(150);
}

void MainWindow::onTrackFinished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
    if (!m_quitting)
        QTimer::singleShot(1000, this, &MainWindow::startTrackDevices);
}

void MainWindow::applyDebouncedRefresh()
{
    for (auto it = m_usbStatusCache.begin(); it != m_usbStatusCache.end(); ++it) {
        const QString &serial = it.key();
        if (serial.contains(QLatin1Char(':')))
            continue;
        if (it.value() != QLatin1String("device"))
            continue;
        if (!m_dataManager->queryDeviceByDaddr(serial).daddr.isEmpty())
            continue;

        QString name = serial;
        if (m_dataManager->descriptionExists(name)) {
            int n = 2;
            do {
                name = QStringLiteral("%1-%2").arg(serial).arg(n);
                n++;
            } while (m_dataManager->descriptionExists(name));
        }

        QSqlQuery query;
        query.prepare(QStringLiteral(
            "INSERT INTO device (description, daddr, port, isusb, ostype, "
            "data_root, xbmcpackage, pulldir, disableroot, filepath, flag5) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));
        query.addBindValue(name);
        query.addBindValue(serial);
        query.addBindValue(QString());
        query.addBindValue(true);
        query.addBindValue(QStringLiteral("0"));
        query.addBindValue(QStringLiteral("/sdcard/"));
        query.addBindValue(QStringLiteral("org.xbmc.kodi"));
        query.addBindValue(QString());
        query.addBindValue(false);
        query.addBindValue(QStringLiteral("files/.kodi"));
        query.addBindValue(QString());
        if (!query.exec())
            logfile("Auto-add USB device failed: " + query.lastError().text());
        else
            logfile("Auto-added USB device: " + serial + " as " + name);
    }

    if (!QApplication::activeModalWidget())
        loadDeviceTableX(deviceTable);
}

void MainWindow::on_actionReload_devices_triggered()
{

   loadDeviceTableX(deviceTable);;

}

/////////////////////////////////////////////////

void MainWindow::on_infoArchitecture_triggered()
{
   infoArchitecture();
}



/////////////////////////////////////////////////////

void MainWindow::on_actionSwitch_View_triggered()
{
   QMessageBox msgBox(this);
   msgBox.setWindowTitle(QStringLiteral("Switch View"));
   msgBox.setText(QStringLiteral("Switch between Kodi and Android view?"));
   msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
   msgBox.setWindowModality(Qt::WindowModal);
   if (msgBox.exec() == QMessageBox::No)
       return;

   // Read existing JSON to preserve other data
   QJsonObject jsonObj;
   QFile jsonFile(databasedir + "/adblink.json");
   if (jsonFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
        if (!doc.isNull()) {
            jsonObj = doc.object();
        }
        jsonFile.close();
   } else {
        logfile("Failed to read adblink.json for updating");
   }

   if (stackedWidget->currentIndex() == 0) {
        stackedWidget->setCurrentIndex(1);
        currentStack = 1;
        menuKodi->menuAction()->setVisible(false);

        infoArchitecture2->setEnabled(false);
        infoArchitecture2->setVisible(false);



        // Set startview to false when currentStack is 1
        jsonObj["startview"] = false;
   } else {
        stackedWidget->setCurrentIndex(0);
        currentStack = 0;
        menuKodi->menuAction()->setVisible(true);

        infoArchitecture2->setEnabled(true);
        infoArchitecture2->setVisible(true);

        // Set startview to true when currentStack is 0
        jsonObj["startview"] = true;
   }

   // Write updated JSON back to file
   if (jsonFile.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(jsonObj);
        jsonFile.write(doc.toJson());
        jsonFile.close();
   } else {
        logfile("Failed to write to adblink.json");

   }
}

///////////////////////////////////



QPushButton* MainWindow::setupDonateButton(QWidget* parent) {
   donateButton = new QPushButton(parent); // Assign to member variable
   QPixmap pix(":/assets/donatel.png");
   if (pix.isNull()) {
        donateButton->setText("Donate");
   } else {
        QIcon icon(pix);
        donateButton->setIcon(icon);
        donateButton->setText("");
        donateButton->setIconSize(QSize(300, 20));
   }
   donateButton->setFixedHeight(26); // Match status bar height
   donateButton->setStyleSheet(
       "QPushButton {"
       "   border: none;"
       "   background: transparent;"
       "   padding: 0;"
       "}"
       "QPushButton:hover {"
       "   background: rgba(200, 200, 200, 50);"
       "}"
        );
    connect(donateButton, &QPushButton::clicked, this, &MainWindow::on_donate_clicked);

    donateButton->setVisible(true);
    donateButton->setEnabled(true);

    return donateButton;
}

/////////////////////////////////////////////////




/////////////////////////////////////////

///////////////////////////////////////

void MainWindow::setupUI() {

centralWidget = new QWidget(this);
setCentralWidget(centralWidget);
mainLayout = new QVBoxLayout(centralWidget);
mainLayout->setContentsMargins(0, 0, 0, 0);
mainLayout->setSpacing(0);

topWidget = new QWidget();
topWidget->setFixedHeight(180);
upperLayout = new QHBoxLayout(topWidget);
upperLayout->setSpacing(0);
upperLayout->setContentsMargins(28, 0, 0, 0);
upperLayout->setAlignment(Qt::AlignTop);



deviceTable->setFixedSize(450, 150);
deviceTable->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

deviceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
deviceTable->horizontalHeader()->setMinimumSectionSize(150);
deviceTable->horizontalHeader()->setMaximumSectionSize(150);
deviceTable->horizontalHeader()->setVisible(true);
deviceTable->verticalHeader()->setVisible(false);
deviceTable->setColumnWidth(0, 150);
deviceTable->setColumnWidth(1, 150);
deviceTable->setColumnWidth(2, 150);
deviceTable->setSelectionMode(QAbstractItemView::SingleSelection);
deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
deviceTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
deviceTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
deviceTable->setFocusPolicy(Qt::ClickFocus);
deviceTable->setShowGrid(true);
deviceTable->setStyleSheet("");
deviceTable->setWordWrap(false);
deviceTable->setTextElideMode(Qt::ElideRight);

QFont tableFont = deviceTable->font();
switch (windowSizeSelector) {
case 0:
        tableFont.setPixelSize(sfontsize);
        break;
case 1:
        tableFont.setPixelSize(mfontsize);
        break;
case 2:
        tableFont.setPixelSize(lfontsize);
        break;
default:
        tableFont.setPixelSize(sfontsize);
        break;
}
deviceTable->setFont(tableFont);
upperLayout->addWidget(deviceTable);

cosmeticGap = new QSpacerItem(12, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
upperLayout->addItem(cosmeticGap);

rightColumnWidget = new QWidget();
rightColumnWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
rightLayout = new QVBoxLayout(rightColumnWidget);
rightLayout->setSpacing(0);
rightLayout->setContentsMargins(0, 0, 0, 0);
rightLayout->setAlignment(Qt::AlignTop);

buttonGridWidget = new QWidget();
buttonGridWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
buttonGridLayout = new QGridLayout(buttonGridWidget);
buttonGridLayout->setSpacing(6);
buttonGridLayout->setContentsMargins(0, 0, 0, 0);

for (int i = 0; i < 6; ++i) {
        buttons[i] = new QPushButton();
        buttons[i]->setFixedSize(ebuttonsize);

        QFont buttonFont = buttons[i]->font();
        switch (windowSizeSelector) {
        case 0:
            buttonFont.setPixelSize(sfontsize);
            break;
        case 1:
            buttonFont.setPixelSize(mfontsize);
            break;
        case 2:
            buttonFont.setPixelSize(lfontsize);
            break;
        default:
            buttonFont.setPixelSize(sfontsize);
            break;
        }
        buttons[i]->setFont(buttonFont);

        buttonGridLayout->addWidget(buttons[i], i / 2, i % 2);

        switch (i) {
        case 0:
            buttons[i]->setText("Connect");
            buttons[i]->setToolTip("Connect selected device (Ctrl+D)");
            break;
        case 1:
            buttons[i]->setText("Disconnect");
            buttons[i]->setToolTip("Disconnect selected device (Ctrl+E)");
            break;
        case 2:
            buttons[i]->setText("New");
            buttons[i]->setToolTip("Add new device (Ctrl+G)");
            break;
        case 3:
            buttons[i]->setText("Edit");
            buttons[i]->setToolTip("Edit selected device (Ctrl+W)");
            break;
        case 4:
            buttons[i]->setText("Delete");
            buttons[i]->setToolTip("Delete selected device (Ctrl+I)");
            break;
        case 5:
            buttons[i]->setText("Clear");
            buttons[i]->setToolTip("Clear ad hoc IP field (Ctrl+J)");
            break;
        }
}

rightLayout->addWidget(buttonGridWidget);

vSpacer = new QSpacerItem(0, 15, QSizePolicy::Minimum, QSizePolicy::Fixed);
rightLayout->addItem(vSpacer);


QHBoxLayout *adhocLayout = new QHBoxLayout();
adhocLayout->setSpacing(6);
adhocLayout->setContentsMargins(0, 0, 0, 0);

QSpacerItem *labelSpacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
adhocLayout->addItem(labelSpacer);

QLabel *adhocLabel = new QLabel("Ad hoc IP:");
adhocLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

QFont labelFont = adhocLabel->font();
switch (windowSizeSelector) {
case 0:
        labelFont.setPixelSize(sfontsize);
        break;
case 1:
        labelFont.setPixelSize(mfontsize);
        break;
case 2:
        labelFont.setPixelSize(lfontsize);
        break;
default:
        labelFont.setPixelSize(sfontsize);
        break;
}
adhocLabel->setFont(labelFont);
adhocLayout->addWidget(adhocLabel);

adhoc_ip = new QLineEdit();
adhoc_ip->setPlaceholderText("IP Address:port");
adhoc_ip->setToolTip("Ad hoc IP: enter IP address then press connect. Add optional port if required.");
adhoc_ip->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
adhocLayout->addWidget(adhoc_ip);

adhocLayout->addStretch();
rightLayout->addLayout(adhocLayout);


upperLayout->addWidget(rightColumnWidget);
mainLayout->addWidget(topWidget);

stackedWidget = new QStackedWidget();
mainLayout->addWidget(stackedWidget);

gridWidget1 = new QWidget();
gridLayout1 = new QGridLayout(gridWidget1);
gridLayout1->setSpacing(0);
gridLayout1->setContentsMargins(0, 0, 0, 0);

for (int i = 0; i < 16; ++i) {
        grid1Buttons[i] = new QPushButton();
        grid1Buttons[i]->setFixedSize(buttonsize);

        QFont grid1ButtonFont = grid1Buttons[i]->font();
        switch (windowSizeSelector) {
        case 0:
            grid1ButtonFont.setPixelSize(sfontsize);
            break;
        case 1:
            grid1ButtonFont.setPixelSize(mfontsize);
            break;
        case 2:
            grid1ButtonFont.setPixelSize(lfontsize);
            break;
        default:
            grid1ButtonFont.setPixelSize(sfontsize);
            break;
        }
        grid1Buttons[i]->setFont(grid1ButtonFont);

        gridLayout1->addWidget(grid1Buttons[i], i / 4, i % 4);

        switch (i) {
        case 0:
            grid1Buttons[i]->setText("File Manager");
            grid1Buttons[i]->setToolTip("Open the file manager (Ctrl+F)");
            break;
        case 1:
            grid1Buttons[i]->setText("ADB Shell");
            grid1Buttons[i]->setToolTip("Open an ADB shell (Ctrl+A)");
            break;
        case 2:
            grid1Buttons[i]->setText("Backup");
            grid1Buttons[i]->setToolTip("Backup device data (Ctrl+B)");
            break;
        case 3:
            grid1Buttons[i]->setText("Restore");
            grid1Buttons[i]->setToolTip("Restore device data (Ctrl+R)");
            break;
        case 4:
            grid1Buttons[i]->setText("Install");
            grid1Buttons[i]->setToolTip("Install an APK file (Ctrl+V)");
            break;
        case 5:
            grid1Buttons[i]->setText("Uninstall");
            grid1Buttons[i]->setToolTip("Uninstall an APK (Ctrl+U)");
            break;
        case 6:
            grid1Buttons[i]->setText("Move Kodi");
            grid1Buttons[i]->setToolTip("Move Kodi data to another location");
            break;
        case 7:
            grid1Buttons[i]->setText("Edit Timers");
            grid1Buttons[i]->setToolTip("Edit device timers");
            break;
        case 8:
            grid1Buttons[i]->setText("Screencap");
            grid1Buttons[i]->setToolTip("Capture the device screen (Ctrl+C)");
            break;
        case 9:
            grid1Buttons[i]->setText("Reset ADB");
            grid1Buttons[i]->setToolTip("Reset the ADB server");
            break;
        case 10:
            grid1Buttons[i]->setText("Scrcpy");
            grid1Buttons[i]->setToolTip("Mirror and control device screen (Ctrl+S)");
            break;
        case 11:
            grid1Buttons[i]->setText("Edit Cache");
            grid1Buttons[i]->setToolTip("Manage device cache");
            break;
        case 12:
            grid1Buttons[i]->setText("Console");
            grid1Buttons[i]->setToolTip("Open the console (Ctrl+N)");
            break;
        case 13:
            grid1Buttons[i]->setText("Keypad");
            grid1Buttons[i]->setToolTip("Access virtual keypad (Ctrl+K)");
            break;
        case 14:
            grid1Buttons[i]->setText("Start App");
            grid1Buttons[i]->setToolTip("Launch an application (Ctrl+O)");
            break;
        case 15:
            grid1Buttons[i]->setText("Stop App");
            grid1Buttons[i]->setToolTip("Stop a running application (Ctrl+P)");
            break;
        }
}

stackedWidget->addWidget(gridWidget1);

gridWidget2 = new QWidget();
gridLayout2 = new QGridLayout(gridWidget2);
gridLayout2->setSpacing(0);
gridLayout2->setContentsMargins(0, 0, 0, 0);

for (int i = 0; i < 12; ++i) {
        grid2Buttons[i] = new QPushButton();
        grid2Buttons[i]->setFixedSize(buttonsize);

        QFont grid2ButtonFont = grid2Buttons[i]->font();
        switch (windowSizeSelector) {
        case 0:
            grid2ButtonFont.setPixelSize(sfontsize);
            break;
        case 1:
            grid2ButtonFont.setPixelSize(mfontsize);
            break;
        case 2:
            grid2ButtonFont.setPixelSize(lfontsize);
            break;
        default:
            grid2ButtonFont.setPixelSize(sfontsize);
            break;
        }
        grid2Buttons[i]->setFont(grid2ButtonFont);

        gridLayout2->addWidget(grid2Buttons[i], i / 4, i % 4);

        switch (i) {
        case 0:
            grid2Buttons[i]->setText("File Manager");
            grid2Buttons[i]->setToolTip("Open the file manager (Ctrl+F)");
            break;
        case 1:
            grid2Buttons[i]->setText("Install APK");
            grid2Buttons[i]->setToolTip("Install an APK file (Ctrl+V)");
            break;
        case 2:
            grid2Buttons[i]->setText("Uninstall APK");
            grid2Buttons[i]->setToolTip("Uninstall an APK (Ctrl+U)");
            break;
        case 3:
            grid2Buttons[i]->setText("System Info");
            grid2Buttons[i]->setToolTip("Display system information");
            break;
        case 4:
            grid2Buttons[i]->setText("Screencap");
            grid2Buttons[i]->setToolTip("Capture the device screen (Ctrl+C)");
            break;
        case 5:
            grid2Buttons[i]->setText("Reset ADB");
            grid2Buttons[i]->setToolTip("Reset the ADB server");
            break;
        case 6:
            grid2Buttons[i]->setText("Start App");
            grid2Buttons[i]->setToolTip("Launch an application (Ctrl+O)");
            break;
        case 7:
            grid2Buttons[i]->setText("Stop App");
            grid2Buttons[i]->setToolTip("Stop a running application (Ctrl+P)");
            break;
        case 8:
            grid2Buttons[i]->setText("ADB Shell");
            grid2Buttons[i]->setToolTip("Open an ADB shell (Ctrl+A)");
            break;
        case 9:
            grid2Buttons[i]->setText("Console");
            grid2Buttons[i]->setToolTip("Open the console (Ctrl+N)");
            break;
        case 10:
            grid2Buttons[i]->setText("Send Text");
            grid2Buttons[i]->setToolTip("Send text to the device (Ctrl+T)");
            break;
        case 11:
            grid2Buttons[i]->setText("ScrCpy");
            grid2Buttons[i]->setToolTip("Mirror and control device screen (Ctrl+S)");
            break;
        }
}

stackedWidget->addWidget(gridWidget2);
stackedWidget->setCurrentIndex(currentStack);


 initGridConnections();


deviceTable->show();
buttonGridWidget->show();
adhoc_ip->show();

loadDeviceTableX(deviceTable);

centralWidget->layout()->activate();
centralWidget->update();
deviceTable->updateGeometry();
deviceTable->viewport()->update();
}


void MainWindow::loadDeviceTableX(QTableWidget* table) {
    m_deviceTableLoader->loadTable(table, windowSizeSelector,
        sfontsize, mfontsize, lfontsize,
        sMainWindowSize, mMainWindowSize, lMainWindowSize,
        [this](const QString &daddr) { return ::usbStatus(m_usbStatusCache, daddr); });
    if (centralWidget) {
        centralWidget->updateGeometry();
        centralWidget->update();
    }
}






///////////////////////////////////////////////////////////


void MainWindow::switchSize()
{
 // Determine the next size based on current windowsize
 if (windowsize == sMainWindowSize) {
   windowsize = mMainWindowSize;
   buttonsize = mGridButtonSize;
   ebuttonsize = m6ButtonSize;
   windowSizeSelector = 1; // Medium
 } else if (windowsize == mMainWindowSize) {
   windowsize = lMainWindowSize;
   buttonsize = lGridButtonSize;
   ebuttonsize = l6ButtonSize;
   windowSizeSelector = 2; // Medium-Large
 } else if (windowsize == lMainWindowSize) {
   windowsize = sMainWindowSize;
   buttonsize = sGridButtonSize;
   ebuttonsize = s6ButtonSize;
   windowSizeSelector = 0; // Small
 } else {
   // Default to Small if windowsize is unknown
   windowsize = sMainWindowSize;
   buttonsize = sGridButtonSize;
   ebuttonsize = s6ButtonSize;
   windowSizeSelector = 0; // Small
 }

 // Apply the new window size
 setFixedSize(windowsize);

 // Save windowSizeSelector to JSON file
 QFile jsonFile(databasedir + "/adblink.json");
 QJsonObject jsonObj;

 // Read existing JSON to preserve other data
 if (jsonFile.open(QIODevice::ReadOnly)) {
   QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
   if (!doc.isNull()) {
        jsonObj = doc.object();
   }
   jsonFile.close();
 } else {
   logfile("Failed to read adblink.json for updating");
 }

 // Update the defaultwindow value with the integer
 jsonObj["defaultwindow"] = windowSizeSelector;

 // Write updated JSON back to file
 if (jsonFile.open(QIODevice::WriteOnly)) {
   QJsonDocument doc(jsonObj);
   jsonFile.write(doc.toJson());
   jsonFile.close();
 } else {
    logfile("Failed to write to adblink.json");
  }

 setupUI();
}

void MainWindow::setWindowSize()
{
 QFile jsonFile(databasedir + "/adblink.json");
 int defaultWindow = 0; // Default to Small (index 0) as fallback

 if (jsonFile.open(QIODevice::ReadOnly)) {
   QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
   jsonFile.close();
   QJsonObject obj = doc.object();
   if (obj.contains("defaultwindow")) {
        QJsonValue value = obj["defaultwindow"];
        if (value.isDouble()) {

               defaultWindow = value.toInt(0); // Fallback to 0 if invalid
               if (defaultWindow < 0 || defaultWindow > 3) {
                    defaultWindow = 0; // Ensure valid index
               }
        }
   }

   int lg = obj["lgfont"].toInt(0);
   int md = obj["mdfont"].toInt(0);
   int sm = obj["smfont"].toInt(0);
   int fm = obj["fmfont"].toInt(0);

   switch (lg) {
   case 0: lfontsize = 14; break;
   case 1: lfontsize = 16; break;
   case 2: lfontsize = 18; break;
   case 3: lfontsize = 20; break;
   default: lfontsize = 14; break;
   }
   switch (md) {
   case 0: mfontsize = 12; break;
   case 1: mfontsize = 14; break;
   case 2: mfontsize = 16; break;
   case 3: mfontsize = 18; break;
   default: mfontsize = 14; break;
   }
   switch (sm) {
   case 0: sfontsize = 10; break;
   case 1: sfontsize = 12; break;
   case 2: sfontsize = 14; break;
   case 3: sfontsize = 16; break;
   default: sfontsize = 12; break;
   }
   switch (fm) {
   case 0: ffontsize = 12; break;
   case 1: ffontsize = 16; break;
   case 2: ffontsize = 18; break;
   case 3: ffontsize = 22; break;
   default: ffontsize = 16; break;
   }

 } else {
   logfile("Failed to read adblink.json");
   defaultWindow = 0;
 }


 switch (defaultWindow) {
 case 0:
   windowsize = sMainWindowSize;
   buttonsize = sGridButtonSize;
   ebuttonsize = s6ButtonSize;
   windowSizeSelector = 0;
   break;
 case 1:
   windowsize = mMainWindowSize;
   buttonsize = mGridButtonSize;
   ebuttonsize = m6ButtonSize;
   windowSizeSelector = 1;
   break;
 case 2:
   windowsize = lMainWindowSize;
   buttonsize = lGridButtonSize;
   ebuttonsize = l6ButtonSize;
   windowSizeSelector = 2;
   break;
 default: // Fallback to Small
   windowsize = sMainWindowSize;
   buttonsize = sGridButtonSize;
   ebuttonsize = s6ButtonSize;
   windowSizeSelector = 0;
   break;
 }

 setFixedSize(windowsize);
 setupUI();
}

void MainWindow::initGridConnections() {
 // Connect statements for buttons in buttonGridLayout
 connect(buttons[0], &QPushButton::clicked, this, &MainWindow::connButton_clicked);
 connect(buttons[1], &QPushButton::clicked, this, &MainWindow::disButton_clicked);
 connect(buttons[2], &QPushButton::clicked, this, [this](bool) { dataentry(true); });
 connect(buttons[3], &QPushButton::clicked, this, [this](bool) { dataentry(false); });
 connect(buttons[4], &QPushButton::clicked, this, &MainWindow::delRecordButton_clicked);
 connect(buttons[5], &QPushButton::clicked, this, &MainWindow::on_clearAdhocButton_clicked);

 // Connect statements for grid1Buttons
 connect(grid1Buttons[0], &QPushButton::clicked, this, &MainWindow::fmButton_clicked);
 connect(grid1Buttons[1], &QPushButton::clicked, this, &MainWindow::adbshellButton_clicked);
 connect(grid1Buttons[2], &QPushButton::clicked, this, &MainWindow::backupButton_clicked);
 connect(grid1Buttons[3], &QPushButton::clicked, this, &MainWindow::restoreButton_clicked);
 connect(grid1Buttons[4], &QPushButton::clicked, this, &MainWindow::sideload_Button_clicked);
 connect(grid1Buttons[5], &QPushButton::clicked, this, &MainWindow::uninstall_Button_clicked);
 connect(grid1Buttons[6], &QPushButton::clicked, this, &MainWindow::mvdataButton_clicked);
 connect(grid1Buttons[7], &QPushButton::clicked, this, &MainWindow::pushTimers_clicked);
 connect(grid1Buttons[8], &QPushButton::clicked, this, &MainWindow::screenCap);
 connect(grid1Buttons[9], &QPushButton::clicked, this, &MainWindow::killServer_clicked);
 connect(grid1Buttons[10], &QPushButton::clicked, this, &MainWindow::scpyButton_clicked);
 connect(grid1Buttons[11], &QPushButton::clicked, this, &MainWindow::cacheButton_clicked);
 connect(grid1Buttons[12], &QPushButton::clicked, this, &MainWindow::doConsole_clicked);
 connect(grid1Buttons[13], &QPushButton::clicked, this, &MainWindow::keypadButton_clicked);
 connect(grid1Buttons[14], &QPushButton::clicked, this, &MainWindow::startapp_clicked);
 connect(grid1Buttons[15], &QPushButton::clicked, this, &MainWindow::stopapp_clicked);

 // Connect statements for grid2Buttons
 connect(grid2Buttons[0], &QPushButton::clicked, this, &MainWindow::fmButton_clicked);
 connect(grid2Buttons[1], &QPushButton::clicked, this, &MainWindow::sideload_Button_clicked);
 connect(grid2Buttons[2], &QPushButton::clicked, this, &MainWindow::uninstall_Button_clicked);
 connect(grid2Buttons[3], &QPushButton::clicked, this, &MainWindow::infoArchitecture);
 connect(grid2Buttons[4], &QPushButton::clicked, this, &MainWindow::screenCap);
 connect(grid2Buttons[5], &QPushButton::clicked, this, &MainWindow::killServer_clicked);
 connect(grid2Buttons[6], &QPushButton::clicked, this, &MainWindow::startapp_clicked);
 connect(grid2Buttons[7], &QPushButton::clicked, this, &MainWindow::stopapp_clicked);
 connect(grid2Buttons[8], &QPushButton::clicked, this, &MainWindow::adbshellButton_clicked);
 connect(grid2Buttons[9], &QPushButton::clicked, this, &MainWindow::doConsole_clicked);
 connect(grid2Buttons[10], &QPushButton::clicked, this, &MainWindow::on_actionSend_text_triggered);
 connect(grid2Buttons[11], &QPushButton::clicked, this, &MainWindow::scpyButton_clicked);
}


//////////////////////////////////

void MainWindow::setupMenus()
{
    setupMenuBar();
    setupFileMenu();
    setupKodiMenu();
    setupUtilityMenu();
    setupAboutMenu();
    setupHelpMenu();
    menuBar->addMenu(menuFile);
    menuBar->addMenu(menuKodi);
    menuBar->addMenu(menuUtility);
    menuBar->addMenu(menuAbout);
    menuBar->addMenu(menuHelp);
    setupMenuConnections();
}

void MainWindow::setupMenuBar()
{
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
}

void MainWindow::setupFileMenu()
{
    menuFile = new QMenu("File", this);
    actionPreferences = new QAction("Preferences", this);
    actionView_adbLink_Log = new QAction("View adblink Log", this);
    actionQuit = new QAction("Quit", this);
    menuFile->addAction(actionPreferences);
    menuFile->addAction(actionView_adbLink_Log);
    menuFile->addSeparator();
    menuFile->addAction(actionQuit);
}

void MainWindow::setupKodiMenu()
{
    menuKodi = new QMenu("Kodi", this);
    actionView_Kodi_Log = new QAction("View Kodi Log", this);
    actionEdit_XML = new QAction("Edit XML", this);
    actionSplash_Screen = new QAction("Splash Screen", this);
    actiondelthumb = new QAction("Delete thumbnails", this);
    actionPush_remote = new QAction("Push remote", this);
    actionDownload_Kodi = new QAction("Download Kodi", this);
    actionKodi_data_usage = new QAction("Kodi data size", this);
    actionKodi_version = new QAction("Check Kodi version", this);
    actionCreate_kodi_data = new QAction("Create kodi_data", this);
    menuKodi->addAction(actionView_Kodi_Log);
    menuKodi->addAction(actionEdit_XML);
    menuKodi->addAction(actionSplash_Screen);
    menuKodi->addAction(actiondelthumb);
    menuKodi->addAction(actionPush_remote);
    menuKodi->addAction(actionDownload_Kodi);
    menuKodi->addAction(actionKodi_version);
    menuKodi->addAction(actionKodi_data_usage);
    menuKodi->addAction(actionCreate_kodi_data);
}

void MainWindow::setupUtilityMenu()
{
    menuUtility = new QMenu("Utility", this);
    actionSwitch_View = new QAction("Switch View", this);
    actionReiinstall_Busybox = new QAction("Reinstall Busybox", this);
    infoArchitecture2 = new QAction("System information", this);
    Erase_adbLink_database = new QAction("Initialize adblink", this);
    actionSend_text = new QAction("Send text to device", this);
    actionGet_UID_from_APK_file = new QAction("Get package name", this);
    actionReload_devices = new QAction("Reload devices", this);
    View_Changelog = new QAction("View changelog", this);
    actionWireless_ADBD = new QAction("Wireless ADBD", this);
    actionReboot = new QAction("Reboot device", this);
    menuUtility->addAction(actionSwitch_View);
    menuUtility->addAction(actionReiinstall_Busybox);
    menuUtility->addAction(infoArchitecture2);
    menuUtility->addAction(Erase_adbLink_database);
    menuUtility->addAction(actionSend_text);
    menuUtility->addAction(actionGet_UID_from_APK_file);
    menuUtility->addAction(actionReload_devices);
    menuUtility->addAction(View_Changelog);
    menuUtility->addAction(actionWireless_ADBD);
    menuUtility->addAction(actionReboot);
}

void MainWindow::setupAboutMenu()
{
    menuAbout = new QMenu("About", this);
    actionAbout = new QAction("About", this);
    menuAbout->addAction(actionAbout);
}

void MainWindow::setupHelpMenu()
{
    menuHelp = new QMenu("Help", this);
    actionHelp = new QAction("Help", this);
    menuHelp->addAction(actionHelp);
}

void MainWindow::setupMenuConnections()
{
    connect(actionPreferences,           &QAction::triggered, this, &MainWindow::on_actionPreferences_triggered);
    connect(actionView_adbLink_Log,     &QAction::triggered, this, &MainWindow::on_actionView_adbLink_Log_triggered);
    connect(actionQuit,                 &QAction::triggered, this, &MainWindow::on_actionQuit_triggered);
    connect(actionView_Kodi_Log,        &QAction::triggered, this, &MainWindow::on_actionView_Kodi_Log_triggered);
    connect(actionEdit_XML,             &QAction::triggered, this, &MainWindow::on_actionEdit_XML_triggered);
    connect(actionSplash_Screen,        &QAction::triggered, this, &MainWindow::on_actionSplash_Screen_triggered);
    connect(actiondelthumb,             &QAction::triggered, this, &MainWindow::on_actiondelthumb_triggered);
    connect(actionPush_remote,          &QAction::triggered, this, &MainWindow::on_actionPush_remote_triggered);
    connect(actionDownload_Kodi,        &QAction::triggered, this, &MainWindow::on_actionDownload_Kodi_triggered);
    connect(actionKodi_data_usage,      &QAction::triggered, this, &MainWindow::on_actionKodi_data_usage_triggered);
    connect(actionKodi_version   ,      &QAction::triggered, this, &MainWindow::on_actionKodi_version);
    connect(actionCreate_kodi_data,     &QAction::triggered, this, &MainWindow::on_actionCreate_kodi_data_triggered);
    connect(actionSwitch_View,          &QAction::triggered, this, &MainWindow::on_actionSwitch_View_triggered);
    connect(actionReiinstall_Busybox, &QAction::triggered, this, [this](bool) {
        ensureBusyboxInstalled(this, getadb(), "Install Busybox?");
    });
    connect(infoArchitecture2,           &QAction::triggered, this, &MainWindow::on_infoArchitecture_triggered);
    connect(Erase_adbLink_database,     &QAction::triggered, this, &MainWindow::on_Erase_adbLink_database_triggered);
    connect(actionSend_text,            &QAction::triggered, this, &MainWindow::on_actionSend_text_triggered);
    connect(actionGet_UID_from_APK_file,&QAction::triggered, this, &MainWindow::on_actionGet_UID_from_APK_file_triggered);
    connect(actionReload_devices,       &QAction::triggered, this, &MainWindow::on_actionReload_devices_triggered);
    connect(View_Changelog,             &QAction::triggered, this, &MainWindow::on_View_Changelog_triggered);
    connect(actionWireless_ADBD,        &QAction::triggered, this, &MainWindow::on_actionWireless_ADBD_triggered);
    connect(actionReboot,               &QAction::triggered, this, &MainWindow::on_actionReboot_triggered);
    connect(actionAbout,                &QAction::triggered, this, &MainWindow::on_actionAbout_triggered);
    connect(actionHelp,                 &QAction::triggered, this, &MainWindow::on_actionHelp_triggered);
}
