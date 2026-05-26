    #include "mainwindow.h"
    #include <QRegularExpression>
    #include "about.h"
    #include "helpdialog.h"
    #include "connectadb.h"
    #include "uninstalldialog.h"
    #include "getreturncode.h"
    #include "editordialog.h"
    #include "keyboarddialog.h"
    #include "usbfiledialog.h"
    #include "listfiledialog.h"
    #include "datadialog.h"
    #include "forcequitdialog.h"
    #include "restdialog.h"
    #include "logdialog.h"
    #include "tcpipdialog.h"
    #include "adbprefdialog.h"
    #include "sleepdialog.h"
    #include "returncode.h"
    #include "oculusdialog.h"
    #include "scpdialog.h"
    #include "program.h"
    #include "getadbdata.h"
    #include "logfile.h"
    #include "adbutils.h"
    #include "getlocaladb.h"
    #include "version.h"
    #include "point.h"
#include "adbconnection.h"
#include "adbdevice.h"
#include "backupmanager.h"
#include "cachemanager.h"
#include "consolemanager.h"
#include "datamovemanager.h"
#include "filemanager.h"
#include "splashscreenmanager.h"
#include "timermanager.h"
#include "uninstallmanager.h"
#include "xmleditormanager.h"
#include "kodidatamanager.h"
#include "kodidownloader.h"
#include "kodiarchdialog.h"
#include "kodisetupmanager.h"
    #include "oculusmanager.h"
    #include "preferencesmanager.h"
#include "remotepushmanager.h"
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
    #include <QtSql>
    #include <QtSql/QSqlDatabase>
    #include <QtSql/QSqlError>
    #include <QtSql/QSqlQuery>
    #include <preferencesdialog.h>
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
    #include <QtNetwork>
    #include <QtNetwork/QNetworkInterface>
    #include <adblogdialog.h>
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

    #ifdef Q_OS_LINUX
     int os=0;
    #elif defined(Q_OS_WIN)
      int os=1;
    #elif defined(Q_OS_MAC)
    int os=2;
    #endif



     MainWindow::MainWindow(QWidget *parent)
         : QMainWindow(parent)
         , m_networkManager(new QNetworkAccessManager(this))
         , m_adbConnection(new AdbConnection(this))
         , m_dataManager(new KodiDataManager(this))
          , m_consoleManager(new ConsoleManager(this))
           , m_backupManager(new BackupManager(this))
           , m_cacheManager(new CacheManager(this))
           , m_fileManager(new FileManager(this))
           , m_dataMoveManager(new DataMoveManager(this))
           , m_splashScreenManager(new SplashScreenManager(this))
           , m_timerManager(new TimerManager(this))
           , m_uninstallManager(new UninstallManager(this))
           , m_xmlEditor(new XmlEditorManager(this))
           , m_kodiDownloader(new KodiDownloader(this))
           , m_kodiSetupManager(new KodiSetupManager(this))
           , m_remotePushManager(new RemotePushManager(this))
     {



        adbfiles=QCoreApplication::applicationDirPath()+"/adbfiles/";
        aapt=QCoreApplication::applicationDirPath()+"/adbfiles/"+"aapt";
        aapt = '"'+aapt+'"';
        apphome = QCoreApplication::applicationDirPath();
        scrcpydir=QCoreApplication::applicationDirPath()+"/adbfiles/"+"scrcpy/";
        xmldir = adbfiles+"remotes/";
        splashdir = adbfiles+"splash/";

        m_dataManager->os = os;
        m_dataManager->logFileDir = logfiledir;
        m_backupManager->setDataManager(m_dataManager);

        connect(m_kodiDownloader, &KodiDownloader::logMessage, this, [](const QString &msg) {
            logfile(msg);
        });


       if (!QFile::exists(adbfiles + "adb") && !QFile::exists(adbfiles + "adb.exe")) {
           QMessageBox::critical(0, "", "adb binary missing!\n", QMessageBox::Cancel);

     }



        if (os == 1) {
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
        QString donation = m_dataManager->readDonationValue(jsonstring);
        setDonateButtonActive(donation != "jocala.com");

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


      logfile(program+" "+version+point);

      if (os == 1) {
            logfile("Windows");
      } else if (os == 2) {
            logfile("macOS");
      } else if (os == 0) {
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
            QMessageBox::critical(0, qApp->tr("Cannot open database"),
                                  QString("Failed to open database:\n%1\nError: %2").arg(dbstring, errorMsg),
                                  QMessageBox::Cancel);
            return;
        }




            deviceTable = new NoHScrollTableWidget(this);

             m_dataManager->createTables();
             m_dataManager->createJsonConfig(jsonstring);
            setupMenus();


            setWindowSize();


            do_versioncheck();




     }





         MainWindow::~MainWindow()
         {



         }



//////////////////////////////////////////////

    void MainWindow::onApplicationQuit() {


     QString cstring = "null kill-server";
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



    void MainWindow::setDonateButtonActive(bool active) {
         if (donateButton) {
      donateButton->setVisible(active);
      donateButton->setEnabled(active);
         } else {
      // qDebug() << "Error: donateButton is not initialized";
         }
    }



    QString MainWindow::readDonationValue() {
        return m_dataManager->readDonationValue(jsonstring);
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

    bool MainWindow::isScoped()
    {


       // Validate getadb()
       QString adbPath = getadb();
       if (adbPath.isEmpty()) {
            logfile("Issue: getadb() returned empty path");
            return false;
       }

       // Helper to run ADB commands
       auto runAdbCommand = [adbPath](const QString& adbCommand) -> QString {
           QString command = adbPath + " " + adbCommand;
           QProcess process;
           process.start(command);
           if (!process.waitForFinished(5000)) {
               logfile("Issue: ADB command timed out: " + command);
               return QString();
           }
           QString output = process.readAllStandardOutput().trimmed();
           QString error = process.readAllStandardError().trimmed();
           if (process.exitCode() != 0 || !error.isEmpty()) {

                // if (!command.contains("Permission denied"))
                //   logfile("Issue: ADB command failed: " + command + " Error: " + error);

               return error.isEmpty() ? "Unknown error" : error;
           }
           return output;
       };

       // Get API level
       QString apiOutput = runAdbCommand("shell getprop ro.build.version.sdk");
       bool ok;
       int apiLevel = apiOutput.toInt(&ok);
       if (!ok || apiOutput.isEmpty()) {
            //logfile("Issue: Invalid or empty API level output: " + apiOutput);
            return false;
       }
       if (apiLevel < 29) {
         //   logfile("Issue: API level too low for scoped storage: " + QString::number(apiLevel));
            return false;
       }




       // Test storage access
       bool restrictedAccess = false;
       QString touchOutput = runAdbCommand("shell touch /sdcard/Android/data/org.xbmc.kodi/files/test.txt");
       if (touchOutput.isEmpty() && !restrictedAccess) {
            // Touch succeeded, clean up
            runAdbCommand("shell rm /sdcard/Android/data/org.xbmc.kodi/files/test.txt");
       } else {
            restrictedAccess = touchOutput.contains("Permission denied", Qt::CaseInsensitive);
            if (!restrictedAccess && !touchOutput.isEmpty()) {
              //   logfile("Issue: Unexpected touch output for primary path: " + touchOutput);
            }
       }

       // Additional test for another path
       if (!restrictedAccess) {
            touchOutput = runAdbCommand("shell touch /sdcard/DCIM/test.txt");
            if (touchOutput.isEmpty()) {
                 // Touch succeeded, clean up
                 runAdbCommand("shell rm /sdcard/DCIM/test.txt");
            } else {
                 restrictedAccess = touchOutput.contains("Permission denied", Qt::CaseInsensitive);
                 if (!restrictedAccess && !touchOutput.isEmpty()) {
                   // logfile("Issue: Unexpected touch output for DCIM path: " + touchOutput);
                 }
            }
       }

       // Check filesystem permissions
       QString lsOutput = runAdbCommand("shell ls -ld /sdcard/");
       if (lsOutput.isEmpty()) {
           logfile("Issue: Failed to get /sdcard/ permissions");
       } else {
            bool permissiveFs = lsOutput.contains("rwxrwxrwx");
            if (permissiveFs) {
               logfile("Issue: Permissive /sdcard/ permissions, vendor may bypass scoped storage");
                 restrictedAccess = false;
            }
       }

       bool result = (apiLevel >= 30) || (apiLevel == 29 && restrictedAccess);
  //   logfile(QString("scoped storage is %1").arg(result ? "in effect" : "not in effect"));
       return result;
    }



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
    void MainWindow::delayTimer(int rdelay)
    {

     QElapsedTimer rtimer;

    int nMilliseconds;
     int i = 0;

     rtimer.start();

    while(i == 0)
      {
        qApp->processEvents();
         nMilliseconds = rtimer.elapsed();
       if (nMilliseconds >= rdelay)
           break;
    }


    }


    //////////////////////////////////////////////
    void MainWindow::rotate_logfile()

    {

     QFile file(logfiledir+"adblink.old.log");

     if( file.exists() )
         QFile::remove(logfiledir+"adblink.old.log");


    QFile file2(logfiledir+"adblink.log");

    if( file2.exists() )
        file2.rename(logfiledir+"adblink.old.log");


    }


    //////////////////////////////////////////////
    void MainWindow::logfile2(QString line)

    {


    QFile file(logfiledir+"adblink.log");
       if(!file.open(QFile::WriteOnly | QFile::Text | QFile::Append))
          {
           QMessageBox::critical(0, "","Can't create logfile!\n",QMessageBox::Cancel);
           return;
          }

       QTextStream out(&file);
                out  << line << Qt::endl;

     }


    /////////////////////////////////////////
    void MainWindow::rebootDevice(QString reboot)
    {

     QElapsedTimer rtimer;
     int nMilliseconds;

    QProcess reboot_device;
    rtimer.start();
    reboot_device.setProcessChannelMode(QProcess::MergedChannels);

    QString cstring = getadb() + " " + reboot;

    reboot_device.start(cstring);
    reboot_device.waitForStarted();
    while(reboot_device.state() != QProcess::NotRunning)
      {
        qApp->processEvents();
         nMilliseconds = rtimer.elapsed();
       if (nMilliseconds >= 5000)
           break;
    }


    }


    /////////////////////////////////////////////////////
    bool MainWindow::mount_system(QString mnt)
    {



        QString cstring;
        QString command;


        cstring = getadb() + " shell /data/local/tmp/adblink/which su";
        command=getadbOutput(cstring);



          cstring = getadb() + " shell su -c /data/local/tmp/adblink/mount  -o "+ mnt + ",remount /";

          command=getadbOutput(cstring);



            if (command.isEmpty())
              {

                return true;
              }
                else
              {

                return false;
              }

    }



    /////////////////////////////
    bool MainWindow::is_su()
    {

    QString cstring = getadb() + " shell /data/local/tmp/adblink/which su";
    QString command=getadbOutput(cstring);



    if (!command.contains("su"))
      {

        return false;
       }
        else
      {
         return true;
       }


    }



    ////////////////////////////////////////////////
     QString MainWindow::strip (QString str)
    {
        str = str.simplified();
        str.replace( " ", "" );
        return str;
    }



     //////////////////////////////////////////
     void MainWindow::kill_server()
     {

      m_adbConnection->killServer();

     }





     //////////////////////////////////

     bool MainWindow::isConnectedToNetwork()
    {
        QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
        bool result = false;

        for (int i = 0; i < ifaces.count(); i++)
        {
            QNetworkInterface iface = ifaces.at(i);
            if ( iface.flags().testFlag(QNetworkInterface::IsUp)
                 && !iface.flags().testFlag(QNetworkInterface::IsLoopBack) )
            {


                // this loop is important
                for (int j=0; j<iface.addressEntries().count(); j++)
                {

                    // we have an interface that is up, and has an ip address
                    // therefore the link is present

                    // we will only enable this check on first positive,
                    // all later results are incorrect

                    if (result == false)
                        result = true;
                }
            }

        }

        return result;
    }




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
        QJsonObject obj;
        QJsonDocument doc(obj);
        QFile file(databasedir + "adblink.json");
        if (file.open(QIODevice::ReadOnly)) {
            doc = QJsonDocument::fromJson(file.readAll());
            obj = doc.object();
            file.close();
        }
        bool checkversion = obj["checkversion"].toBool();
        bool startview = obj["startview"].toBool();

        if (startview) {
            stackedWidget->setCurrentIndex(0);
            currentStack=0;
            menuKodi->menuAction()->setVisible(true);
            infoArchitecture2->setEnabled(true);
            infoArchitecture2->setVisible(true);

        } else {
            stackedWidget->setCurrentIndex(1);
            currentStack=1;
            menuKodi->menuAction()->setVisible(false);
            infoArchitecture2->setEnabled(false);
            infoArchitecture2->setVisible(false);
        }

        if (checkversion) {
            if (!QUrl(vqurl).isValid()) {
                QMessageBox::critical(this, "", "Invalid URL for version check", QMessageBox::Cancel);
                return;
            }
            QNetworkRequest request;
            request.setUrl(QUrl(vqurl));
            QNetworkReply *reply = m_networkManager->get(request);
            connect(reply, &QNetworkReply::finished, this, &MainWindow::onReqCompleted);
        }
    }

    void MainWindow::onReqCompleted()
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        if (!reply) {
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::critical(this, "", "Network error: " + reply->errorString(), QMessageBox::Cancel);
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





    ////////////////////////////////////////////////////////////////////////////
    bool MainWindow::installAPK(QString filename)
    {


        QString command;
        QString cstring;
        QString port;
        QString daddr;
        QString selectedDescription = deviceTable->item(deviceTable->currentRow(), 0)->text();




        DeviceRecord device = queryDeviceRecord(selectedDescription);




        logfile("Installing "+filename);


         cstring = getadb() + " install -r " + '"'+ filename+'"';

        command=RunLongProcess(cstring,"installing apk(s)");
        logfile(cstring);
        logfile(command);



        if (!command.contains("uccess") || command.contains("Failure"))
        {
            QMessageBox::critical(this,"",filename+" install failed.\nSee log.");
            return false;
        }
        else return true;




    }

    ////////////////////////////////////////////////////////////////////////////
    void MainWindow::sideload_Button_clicked()
    {





        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
            return;
        }



        DeviceRecord device = queryDeviceRecord(selectedDescription);


         bool installer=false;


         QString install = readInstall(databasedir);

         QStringList filenames = QFileDialog::getOpenFileNames(this, tr("APK files (*.apk);;All files (.*)"), install);



        if( !filenames.isEmpty() )
        {

            QMessageBox::StandardButton reply;
              reply = QMessageBox::question(this, "Install", "Install APKs?",
                                            QMessageBox::Yes|QMessageBox::No);
              if (reply == QMessageBox::Yes)
              {

                logfile("starting APK installation(s)");
                for (int i =0;i<filenames.count();i++)
                  installer = installAPK(filenames.at(i));

              }



          //    getadbpath() install --bypass-low-target-sdk-block someapp.apk

              if (installer)
              {


                  writeInstall(filenames[0].left(filenames[0].lastIndexOf('/')));
                  QMessageBox::information(this,"","APK(s) installed.\nSee log for details.");

                  install = filenames[0].left(filenames[0].lastIndexOf('/'));
                  writeInstall(install);


              }


        }


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


    void MainWindow::connButton_clicked()
    {
                      QString adhoc="Ad hoc";
                      QString cstring;
                      QString command;
                      QString s;
                      QString selectedDescription;
                      QString daddr;
                      QString port;

                      int selectedRow;

                      if (!adhoc_ip->text().isEmpty())
                      {
                            adhocip();
                            adhoc_ip->clear();
                            for (int row = 0; row < deviceTable->rowCount(); ++row) {
                              QTableWidgetItem* item = deviceTable->item(row, 0);
                              if (item && item->text() == adhoc) {
                  deviceTable->selectRow(row);
                  break;
                              }
                            }
                      }

                      selectedRow = deviceTable->currentRow();
                      if (selectedRow >= 0 && deviceTable->item(selectedRow, 0)) {
                            selectedDescription = deviceTable->item(selectedRow, 0)->text();
                      } else {
                            QMessageBox::critical(this, "", "No device selected in table");
                             return;
                       }

                       DeviceRecord device = queryDeviceRecord(selectedDescription);

                       if (device.isusb) {
                            logfile("USB connection attempted, not supported");
                            QMessageBox::critical(this, "", "Inactive for USB connections");
                            return;
                      }

                      if (!validateIPAddress(device.daddr)) {
                            QMessageBox::critical(this, "Error", "Invalid IP address");
                            return;
                      }

                      port = device.port.isEmpty() ? "5555" : device.port;
                      daddr = device.daddr + ":" + port;


                       command = connectadb(getadbpath(), QStringList() << "connect" << daddr);


                      if (command.contains("failed to authenticate") || command.contains("offline")) {

                            deviceTable->setItem(selectedRow, 2, new QTableWidgetItem(
                                                                         command.contains("failed to authenticate") ? "Unauthorized" : "Offline"));
                            logfile(cstring);
                            logfile(command);
                            QString cstring = getadbpath() + " disconnect " + daddr;
                            command = connectadb(getadbpath(), QStringList() << "disconnect" << daddr);
                            // command = connectadb(cstring);
                            return;
                      }



                      if (command.contains("connected to")) {

                            deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Connected"));

                            deviceTable->clearSelection();
                            deviceTable->setCurrentCell(selectedRow, 0);
                            deviceTable->selectRow(selectedRow);
                            deviceTable->setFocus();

                            logfile("Connected to " + daddr);
                            // logfile("Android version: " + s.setNum(getandroid()));
                            infolog();

                      } else {

                            deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("NA"));
                            logfile("Unable to connect to: " + daddr);
                            QMessageBox::critical(this, "", "Unable to connect to: " + daddr);
                      }


    }


////////////////////////////////////////////

    void MainWindow::adhocip()
    {
                      QString cstring;
                      QString command;
                      QString daddr;
                      QString port;

                      if (!adhoc_ip->text().isEmpty())
                      {
                            QString adhocIPText = adhoc_ip->text().trimmed();
                            int colonIndex = adhocIPText.indexOf(':');
                            QString daddr, port;

                            if (colonIndex != -1) {
                              daddr = adhocIPText.left(colonIndex).trimmed();
                              port = adhocIPText.mid(colonIndex + 1).trimmed();
                            } else {
                              daddr = adhocIPText;
                              port = "5555";
}


//////////////////////////////////////////

                            bool ok;
                            int portNum = port.toInt(&ok);
                            if (!ok || portNum < 1 || portNum > 65535) {
                              logfile("Invalid port: " + port);
                              QMessageBox::critical(this, "", "Invalid port: " + port);
                              return;
                        }

                            QSqlQuery query;
                            query.prepare("INSERT OR REPLACE INTO device (description, daddr, port, isusb, data_root, xbmcpackage, filepath) "
                                          "VALUES (:description, :daddr, :port, :isusb, :data_root, :xbmcpackage, :filepath)");
                            query.bindValue(":description", "Ad hoc");
                            query.bindValue(":daddr", daddr);
                            query.bindValue(":port", port);
                            query.bindValue(":isusb", 0);
                            query.bindValue(":data_root", "/sdcard/");
                            query.bindValue(":xbmcpackage", "org.xbmc.kodi");
                            query.bindValue(":filepath", "/files/.kodi");
                            if (!query.exec()) {
                              logfile("Failed to insert temporary device: " + query.lastError().text());
                            } else {
                              logfile("Temporary device record inserted: Ad hoc IP, " + daddr + ":" + port);
                            }
                      }

                      loadDeviceTableX(deviceTable);;
    }

    ////////////////////////////////////////////////////////////////

    void MainWindow::disButton_clicked()
    {


             QString daddr;
             int selectedRow = deviceTable->currentRow();

             // Validate selection and connection status
             if (selectedRow < 0 || !deviceTable->item(selectedRow, 2) || !deviceTable->item(selectedRow, 1)) {
            QMessageBox::critical(this, "", "No valid device selected");
            return;
             }
             if (deviceTable->item(selectedRow, 2)->text() != "Connected") {
            QMessageBox::critical(this, "", "Selected device is not connected");
            return;
             }

             if (deviceTable->item(selectedRow, 1)->text().contains("USB")) {
            QMessageBox::critical(this, "", "Inactive for USB connections");
            return;
             }



             daddr = deviceTable->item(selectedRow, 1)->text();



             // Confirm disconnection
             QMessageBox::StandardButton reply;
             reply = QMessageBox::question(this, "Disconnect", "Disconnect device?",
                                           QMessageBox::Yes | QMessageBox::No);
             if (reply == QMessageBox::No) {
            return;
             }


             QString cstring =  "null disconnect " + daddr;

             QString command=getadbOutput(cstring);

             logfile(command);
             logfile("disconnect: " + daddr);






             // Update status column (column 2) to "Disconnected"
             if (selectedRow >= 0 && deviceTable->item(selectedRow, 2)) {
            deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Disconnected"));
             }


    }




    /////////////////////////////////////////////////////////////////////////
    void MainWindow::on_actionAbout_triggered()
    {
             // Read donation value from adblink.json
             QString donation;
             QFile file(databasedir + "adblink.json");
             if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            if (!doc.isNull()) {
                              QJsonObject obj = doc.object();
                              donation = obj["donation"].toString();
            } else {
                              // qDebug() << "Error: Invalid JSON in adblink.json";
            }
            file.close();
             } else {
            // qDebug() << "Error: Could not open adblink.json at" << databasedir;
             }

             // Create Dialog2 and pass the donation value
             Dialog2 dialog2(this, donation);
             dialog2.setWindowModality(Qt::WindowModal);
             dialog2.setaLabel(program + " " + version+point);
             dialog2.exec();
    }

    //////////////////////////////////////////////
    void MainWindow::on_actionHelp_triggered()
    {

        helpDialog helpdialog;


        helpdialog.setWindowModality(Qt::NonModal);
        helpdialog.setWindowTitle("Help");
        helpdialog.exec();
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
          if (!validateDeviceSelection(selectedDescription)) {
           return;
          }

          DeviceRecord device = queryDeviceRecord(selectedDescription);



       QMessageBox::StandardButton reply;
         reply = QMessageBox::question(this, "Reboot Device", "Reboot Device?",
             QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::Yes) {
            logfile("rebooting device");
           rebootDevice(" reboot");

            QString daddr;
            int selectedRow = deviceTable->currentRow();
            daddr = deviceTable->item(selectedRow, 1)->text();

            QString cstring =  "null disconnect " + daddr;

            if (!device.isusb)
            {
            QString command=getadbOutput(cstring);
            logfile(command);
            logfile("disconnect: " + daddr);
            }



            if (selectedRow >= 0 && deviceTable->item(selectedRow, 2)) {
                   deviceTable->setItem(selectedRow, 2, new QTableWidgetItem("Disconnected"));
            }

         }

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
       if (QMessageBox::question(this, "Disconnect", "Disconnect all IPs?", QMessageBox::Cancel | QMessageBox::Ok) == QMessageBox::Cancel) {
            return;
       }

       // Kill the ADB server
       QString cstring = getadbpath() + " kill-server";
       QString command = getadbOutput(cstring);


       // Update device table
       for (int row = 0; row < deviceTable->rowCount(); ++row) {
            QTableWidgetItem* descItem = deviceTable->item(row, 0);
            if (!descItem) continue; // Skip if no item

            // Get the current status from column 2
            QTableWidgetItem* statusItem = deviceTable->item(row, 2);
            QString currentStatus = statusItem ? statusItem->text() : "";

            // Check if the device is USB based on stored data
            bool isUsb = descItem->data(Qt::UserRole + 1).toBool();

            // Only update status to "Disconnected" if it's not already "USB"
            if (currentStatus != "USB") {
                   QString status = isUsb ? "USB" : "Disconnected";
                   QTableWidgetItem* newStatusItem = new QTableWidgetItem(status);
                   deviceTable->setItem(row, 2, newStatusItem);
            }
       }

       deviceTable->viewport()->update();
    }


//////////////////

     void MainWindow::dataentry(bool isNewRecord)
     {
        QString command = getadbOutput("null devices");
        QStringList mstringlist = command.split(QRegularExpression("[\t\n\r]"), Qt::SkipEmptyParts);
        QStringList dstringlist;

        if (command.contains("List of devices attached"))
        {
             mstringlist.removeFirst();
             for (int a = 0; a < mstringlist.size(); a = a + 2)
             {
                    QStringList pieces = mstringlist.at(a).split(":", Qt::SkipEmptyParts);
                    if (!mstringlist.at(a).contains("daemon"))
                   dstringlist << pieces.at(0);
             }
        }

        DeviceRecord device;
        QString selectedDescription;

        if (!isNewRecord)
        {
             int selectedRow = deviceTable->currentRow();
             if (selectedRow >= 0 && deviceTable->item(selectedRow, 0))
             {
                    selectedDescription = deviceTable->item(selectedRow, 0)->text();
                    device = queryDeviceRecord(selectedDescription);
             }
             else
             {
                    QMessageBox::critical(this, "", "No device selected in table");
                    return;
             }
        }

        DeviceEditor editor(this, stackedWidget->currentIndex() == 0,
                            version,
                            [this]() { on_Erase_adbLink_database_triggered(); });
        editor.setDeviceList(dstringlist);
        if (!isNewRecord)
             editor.setExistingDevice(device, selectedDescription);

        if (editor.exec())
        {
             QSqlDatabase::database().commit();
             loadDeviceTableX(deviceTable);
        }
     }


    ///////////////////////////////////////////
     void MainWindow::cacheButton_clicked()
     {
         QString selectedDescription;
         if (!validateDeviceSelection(selectedDescription))
             return;

         DeviceRecord device = queryDeviceRecord(selectedDescription);

         m_cacheManager->configureCache(this, device, getadb(), isScoped(), scriptdir);
     }



    //////////////////////////////////////////////////
    void MainWindow::on_actionView_Kodi_Log_triggered()
    {

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
               return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

            androidLog();

    }

    //////////////////////////////////////////////////
    void MainWindow::androidLog()
    {
          QString selectedDescription;
          if (!validateDeviceSelection(selectedDescription)) {
            return;
          }

          DeviceRecord device = queryDeviceRecord(selectedDescription);



          QString xpath = "";
          QString cstring;
          QString command;
          QString mcpath="";



          mcpath = resolveKodiPath(getadb(), device.data_root, device.xbmcpackage, isScoped());





          xpath = mcpath+"/temp/";


          // qDebug() << xpath;

          cstring = getadb() + " shell "+busypath+"busybox find " +xpath+ " -maxdepth 1 -name kodi.log ";

          command=getadbOutput(cstring);


          if (command.isEmpty() || command.contains("No such file or directory"))
          { QMessageBox::critical(this,"","Kodi log not found");

            // logfile(cstring);
            logfile(command);
            logfile("Kodi log not found!");
            return;
          }






        device.filepath=xpath;

        logfile("opening kodi log");
        logDialog logdialog;
        logdialog.loadRemoteLog(getadb(), device.filepath);
        logdialog.setModal(true);
        logdialog.exec();

    }

    ////////////////////////////////////////////////////
    void MainWindow::on_actionView_adbLink_Log_triggered()
    {
        adblogDialog dialog;
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
    void MainWindow::usbbuttons(bool isusb)
    {

     return;

        if(!isusb)
               {
                 // ui->connButton->setDisabled(false);
                 // ui->disButton->setDisabled(false);
                  }
              else
                  {
                  // ui->connButton->setDisabled(true);
                   // ui->disButton->setDisabled(true);
                  }

    }


    //////////////////////////////////////////////////////

    void MainWindow::on_actionStop_Application_triggered()
    {
                  QString selectedDescription;
                  if (!validateDeviceSelection(selectedDescription)) {
                    return;
                  }

                  DeviceRecord device = queryDeviceRecord(selectedDescription);




          bool startstop;

          QString stopapp;

          if (QFileInfo::exists(databasedir+"/stopapp.json"))
                 startstop = true;
          else
                 startstop = false;

          if (!startstop)
          {



                 QJsonObject obj;
                 obj["stopapp"] = "org.xbmc.kodi";
                 QJsonDocument doc(obj);
                 QFile file(databasedir+"stopapp.json");
                 file.open(QIODevice::WriteOnly);
                 file.write(doc.toJson());
                 file.close();


          }



          QJsonObject obj;
          QJsonDocument doc(obj);
          QFile file(databasedir+"stopapp.json");
          file.open(QIODevice::ReadOnly);
          doc = QJsonDocument::fromJson(file.readAll());
          obj = doc.object();
          stopapp=obj["stopapp"].toString();
          file.close();


        forcequitDialog dialog(false,stopapp,this);
        dialog.setWindowModality(Qt::WindowModal);

        if(dialog.exec() == QDialog::Accepted)
        {


        QString cstring = getadb() + " shell am force-stop "+dialog.packagename();
        QString command=getadbOutput(cstring);
         logfile(cstring);
         logfile(command);

        QJsonObject obj;
        obj["stopapp"] = dialog.packagename();
        QJsonDocument doc(obj);
        QFile file(databasedir+"stopapp.json");
        file.open(QIODevice::WriteOnly);
        file.write(doc.toJson());
        file.close();

        }


    }

    /////////////////////////////////////////////////////////

    void MainWindow::on_actionStart_Application_triggered()
    {

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
        return;
        }

        DeviceRecord device = queryDeviceRecord(selectedDescription);




                bool startstop;
                QString startapp;


                if (QFileInfo::exists(databasedir+"/startapp.json"))
                  startstop = true;
                else
                 startstop = false;

                if (!startstop)
                {
                  QJsonObject obj;
                  obj["startapp"] = "org.xbmc.kodi/org.xbmc.kodi.Splash";
                  QJsonDocument doc(obj);
                  QFile file(databasedir+"startapp.json");
                  file.open(QIODevice::WriteOnly);
                  file.write(doc.toJson());
                  file.close();
                }



                QJsonObject obj;
                QJsonDocument doc(obj);
                QFile file(databasedir+"startapp.json");
                file.open(QIODevice::ReadOnly);
                doc = QJsonDocument::fromJson(file.readAll());
                obj = doc.object();
                startapp=obj["startapp"].toString();
                file.close();



         forcequitDialog dialog(true,startapp, this);
         dialog.setWindowModality(Qt::WindowModal);


        if(dialog.exec() == QDialog::Accepted)
        {

            QString cstring = getadb() + " shell am start -n "+dialog.packagename();
            QString command=getadbOutput(cstring);
            logfile(cstring);
            logfile(command);

            QJsonObject obj;
            obj["startapp"] = dialog.packagename();
            QJsonDocument doc(obj);
            QFile file(databasedir+"startapp.json");
            file.open(QIODevice::WriteOnly);
            file.write(doc.toJson());
            file.close();

        }

    }


    ////////////////////////////////////////////////

    void MainWindow::keypadButton_clicked()
    {



        QString port;
        QString daddr;

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
            return;
        }


        DeviceRecord device = queryDeviceRecord(selectedDescription);




        if (device.port.isEmpty()) {
            port = "5555";
        }
        else
            port=device.port;

        daddr = device.daddr+":"+port;

         QString cstring = daddr + " shell input keyevent ";
         keyboardDialog dialog(this);
         dialog.setWindowModality(Qt::WindowModal);
         dialog.setdaddr(cstring);
         dialog.exec();

    }


    ///////////////////////////////////////////////////////


     void MainWindow::fmButton_clicked()
     {
         QString selectedDescription;
         if (!validateDeviceSelection(selectedDescription))
             return;

         DeviceRecord device = queryDeviceRecord(selectedDescription);

         QFile file(databasedir + "adblink.json");
         file.open(QIODevice::ReadOnly);
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

        int tsvalue = 4000;

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
        timer->start(tsvalue);

        command = getadbOutput(cstring);

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




        serverlabel();

        return command;
    }



    QString MainWindow::RunLongProcess2(QString cstring, QString jobname)
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

        int tsvalue = 4000;

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
        timer->start(tsvalue);

        command = getadbOutput(cstring);

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




        serverlabel();

        return command;
    }


   ////////////////////////////////////////////////////////

    void MainWindow::on_actionMount_system_RO_triggered()
    {


            QString selectedDescription;
            if (!validateDeviceSelection(selectedDescription)) {
             return;
            }

            DeviceRecord device = queryDeviceRecord(selectedDescription);



        if (!is_su())
           {
             QMessageBox::critical(this,"","Root required!");
            return;
           }


        if (mount_system("ro"))
         {

            logfile("/system partition is read-only");

            QMessageBox::information(this,"","/ partition is read-only");
         }

         else

        {
            logfile("filesystem not remounted r/o!");

             QMessageBox::critical(this,"","filesystem not remounted r/o!");
        }


    }

    ////////////////////////////////////////////////////////

    void MainWindow::on_actionMount_system_RW_triggered()
    {

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
             return;
        }

        DeviceRecord device = queryDeviceRecord(selectedDescription);





        if (!is_su())
        {
            QMessageBox::critical(this,"","Root required!");
        return;
        }



        if (mount_system("rw"))

           {

            logfile("/system partition is read-only");

            QMessageBox::information(this,"","/ partition is read-write");
        }
         else

        {
            logfile("filesystem not remounted r/o!");

                QMessageBox::critical(this,"","filesystem not mounted read-write");
        }


    }

    ///////////////////////////////////////////////////////


    void MainWindow::on_actionWireless_ADBD_triggered()
    {
        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
                return;
        }

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        if (!device.isusb) {
                QMessageBox::critical(this, "", "USB devices only!");
                return;
        }

        QStringList args;
        QString command;
        QString cstring;



        cstring = "null -s " +device.daddr+ " shell ip route";
        command = getadbOutput(cstring);

        QString ip;
        {
                QRegularExpression re(R"(src\s+(\d+\.\d+\.\d+\.\d+))");
                QRegularExpressionMatch match = re.match(command);
                if (match.hasMatch())
              ip = match.captured(1);
        }

        logfile("Device IP: " + ip);

        tcpipDialog dialog;


        cstring = "null  -s "+device.daddr+" shell getprop persist.adb.tcp.port";
        command = getadbOutput(cstring);
        logfile("shell getprop persist.adb.tcp.port: " + command);

        dialog.settcplabel("Device IP: " + ip);


        if(dialog.exec() == QDialog::Accepted)
        {

                cstring = "null -s "+device.daddr+" tcpip 5555";
                command = getadbOutput(cstring);
                logfile(command);


                QTimer::singleShot(2000, this, [this, ip]() {

                    // Connect over Wi-Fi
                    QString cstring = "null connect " + ip + ":5555";
                    QString command = getadbOutput(cstring);
                    logfile("adb connect: " + command);


                    if (command.contains("connected to"))
                    {
                        QMessageBox::information(this, "Success",
                                                 "Wireless ADB enabled for " + ip);


                         cstring = "null disconnect " + ip + ":5555";
                        command = getadbOutput(cstring);
                        logfile("adb disconnect: " + command);
                    }
                    else
                    {
                        QMessageBox::warning(this, "Failure",
                                             "Failed to connect over TCP/IP to " + ip);
                    }
                });
        }
    }





    ////////////////////////////////////////////////////////

    void MainWindow::on_actionKodi_data_usage_triggered()
    {
        QString kodidata;
        QString cstring;
        QString command;
        QString xbmcpath;

        QString selectedDescription;
        if (!validateDeviceSelection(selectedDescription)) {
                return;
        }

        DeviceRecord device = queryDeviceRecord(selectedDescription);

        cstring = " -s "+ device.daddr + " shell ls /sdcard/xbmc_env.properties";
        QStringList args = QProcess::splitCommand(cstring);

        if (  returncode(getadbpath(), args)    ) {
                cstring = "null -s "+ device.daddr + " shell cat /sdcard/xbmc_env.properties";
                command = getadbOutput(cstring);


                command.replace(QRegularExpression("[\r\n]"), "");

                int startIndex = command.indexOf("=") + 1;
                int endIndex = command.indexOf(".kodi") + 5;
                xbmcpath = command.mid(startIndex, endIndex - startIndex);

        }

        else

        {



                xbmcpath = "/sdcard/Android/data/"+device.xbmcpackage;

        }


        cstring = getadb() + " shell du -sh " + xbmcpath;


        command=RunLongProcess(cstring,"calculating data size");


        if (command.contains("No such file"))
        {
                kodidata = "No data found";
        }

        else
        {



                int z = command.indexOf("G");

                if (z==-1)
              z = command.indexOf("M");

                if (z==-1)
              z = command.indexOf("K");


                if (z != -1)
              kodidata = command.mid(0,z+1);

        }

        QMessageBox::information(0,"Kodi Data","Kodi data size:  " + kodidata);

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

         QString latestVer = m_kodiDownloader->fetchLatestVersion();

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
             QMessageBox::information(this, "Kodi Version Check", message);
         }
     }



    ////////////////////////////////////////////////////////////////////////

    void MainWindow::on_clearAdhocButton_clicked()
    {
        adhoc_ip->setText("");


    }


////////////////////////////////////////////////////////////

    void MainWindow::on_actionPreferences_triggered()
    {
         PreferencesManager mgr(databasedir + "adblink.json", os);
         if (!mgr.exec(this))
             return;

         PreferencesResult r = mgr.result();
         setDonateButtonActive(r.donationValue != "jocala.com");

         switch (r.lgFontIndex) {
         case 0: lfontsize = 16; break;
         case 1: lfontsize = 18; break;
         case 2: lfontsize = 20; break;
         case 3: lfontsize = 22; break;
         default: lfontsize = 16; break;
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
        QJsonObject obj;
        QJsonDocument doc(obj);
        QFile jsonFile(databasedir + "adblink.json");
        if (jsonFile.open(QIODevice::ReadOnly)) {
            doc = QJsonDocument::fromJson(jsonFile.readAll());
            obj = doc.object();
            jsonFile.close();
        }

        QString downloadDir = obj["install"].toString();
        if (downloadDir.isEmpty())
            downloadDir = QDir::homePath();

        QString kodiVersion = m_kodiDownloader->fetchLatestVersion();

        KodiArchDialog archDialog(kodiVersion, this);
        if (archDialog.exec() != QDialog::Accepted)
            return;

        int arch = archDialog.selectedArch();
        if (arch == 2) {
            QDesktopServices::openUrl(QUrl("https://kodi.tv/download/android"));
            return;
        }

        if (kodiVersion == "Unknown") {
            QMessageBox::critical(this, "Error",
                "Cannot download: Kodi version unknown. See log");
            logfile("Download aborted: Unknown Kodi version");
            return;
        }

        disconnect(m_kodiDownloader, &KodiDownloader::downloadCompleted, this, nullptr);
        disconnect(m_kodiDownloader, &KodiDownloader::downloadFailed, this, nullptr);
        disconnect(m_kodiDownloader, &KodiDownloader::downloadProgress, this, nullptr);

        progressBar->setHidden(false);
        progressBar->setValue(0);
        server_running->setText("Downloading Kodi...");
        container->setHidden(true);

        auto hideProgress = [this]() {
            progressBar->setHidden(true);
            progressBar->setValue(0);
            container->setHidden(false);
            server_running->setText("");
            serverlabel();
        };

        connect(m_kodiDownloader, &KodiDownloader::downloadProgress,
                this, [this](qint64 received, qint64 total) {
            if (total > 0)
                progressBar->setValue(static_cast<int>(received * 100 / total));
        });

        connect(m_kodiDownloader, &KodiDownloader::downloadCompleted,
                this, [this, hideProgress](const QString &filePath) {
            hideProgress();
            logfile("The Kodi APK file has been downloaded successfully to:\n" + filePath);

            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Download Success");
            msgBox.setText("Kodi downloaded. See log for details");
            QAbstractButton *installBtn = msgBox.addButton("Install", QMessageBox::ActionRole);
            QAbstractButton *logBtn = msgBox.addButton("Logfile", QMessageBox::ActionRole);
            msgBox.addButton(QMessageBox::Ok);
            msgBox.exec();

            if (msgBox.clickedButton() == installBtn) {
                QString desc;
                if (validateDeviceSelection(desc)) {
                    DeviceRecord device = queryDeviceRecord(desc);
                    installAPK(filePath);
                }
            } else if (msgBox.clickedButton() == logBtn) {
                on_actionView_adbLink_Log_triggered();
            }
        });

        connect(m_kodiDownloader, &KodiDownloader::downloadFailed,
                this, [hideProgress](const QString &error) {
            hideProgress();
            logfile("Kodi download failed: " + error);
            QMessageBox::critical(nullptr, "Download Failed",
                "Failed to download Kodi. See log");
        });

        m_kodiDownloader->startDownload(kodiVersion, arch, downloadDir);
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
    QString scoped = isScoped() ? "true" : "false";
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
    QString scoped = isScoped() ? "true" : "false";
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
            if (!validateDeviceSelection(selectedDescription)) {
                return;
            }

            DeviceRecord device = queryDeviceRecord(selectedDescription);

        QString cstring;
        QString command;
        QString mcpath;




        mcpath = resolveKodiPath(getadb(), "/sdcard/", device.xbmcpackage, false);





        QString thumb = mcpath+"/userdata/Thumbnails";
        QString textures = mcpath+"/userdata/Database/Textures*.db";

        cstring = getadb() +" shell ls "+thumb;
        command=getadbOutput(cstring);



       if (command.contains("No such file or directory"))
        {
              QMessageBox::critical(this,"","Thumbnails not found!");
              return;
       }







        QMessageBox::StandardButton reply2;
           reply2 = QMessageBox::question(this, "", "Delete Thumbnails?",
                                        QMessageBox::Yes|QMessageBox::No);
           if (reply2 == QMessageBox::Yes)
             {
               logfile("Removing Thumbnails");
               cstring = getadb() +" shell rm -r " + thumb;
               command=RunLongProcess(cstring,"Removing Thumbnails");
               if (command.length() > 0)
                  logfile("Thumbnail directory issue: "+command);
               cstring = getadb() +" shell rm -r " + textures;
               command=getadbOutput(cstring);
               if (command.length() > 0)
                  logfile("Textures database issue: "+command);
           }

           if (command.length() > 0)
              QMessageBox::critical(this,"","Errors. See log");
            else
              QMessageBox::information(this,"","Thumnails deleted");


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
    if (m_isBusy) return;
    m_isBusy = true;

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
        m_isBusy = false;
        return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    if (!::isPackageInstalled(getadb(), device.xbmcpackage)) {
        QMessageBox::critical(this, "", device.xbmcpackage + " not installed");
        logfile(device.daddr + ": Error: " + device.xbmcpackage + " not installed");
        m_isBusy = false;
        return;
    }

    const QString adbPrefix = getadb() + " ";

    m_backupManager->backupDevice(this, device, adbPrefix, isScoped(),
                                  m_dataManager->os, jsonstring, m_dataManager,
                                  [this](const QString &cmd, const QString &title) {
                                      return RunLongProcess(cmd, title);
                                  });

    m_isBusy = false;
}


/////////////////////////////////////////////

void MainWindow::restoreButton_clicked() {
    if (m_isBusy) return;
    m_isBusy = true;

    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription)) {
        m_isBusy = false;
        return;
    }

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    if (!::isPackageInstalled(getadb(), device.xbmcpackage)) {
        QMessageBox::critical(this, "", device.xbmcpackage + " not installed");
        logfile(device.daddr + ": Error: " + device.xbmcpackage + " not installed");
        m_isBusy = false;
        return;
    }

    const QString adbPrefix = getadb() + " ";

    m_backupManager->restoreDevice(this, device, adbPrefix, isScoped(),
                                   jsonstring, m_dataManager,
                                   [this](const QString &cmd, const QString &title) {
                                       return RunLongProcess(cmd, title);
                                   });

    m_isBusy = false;
}


//////////////////////////////////////////////
void MainWindow::mvdataButton_clicked()
{
    QString selectedDescription;
    if (!validateDeviceSelection(selectedDescription))
        return;

    DeviceRecord device = queryDeviceRecord(selectedDescription);

    m_dataMoveManager->moveKodiData(this, device, getadb(), isScoped(),
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


QString MainWindow::checkslash(QString qpath)
{

    if(!qpath.startsWith("/"))
     qpath.prepend("/");

    if(!qpath.endsWith("/"))
     qpath.append("/") ;

    return qpath;
}


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


 QString command;
 QString cstring;
 QStringList mstringlist;



 QString filename = QFileDialog::getOpenFileName(
    this,
    "Open APK File",
    QDir::homePath(),
     "APK Files (*.apk);;All Files (*)"
     );


 if( !filename.isEmpty() )
 {

            cstring = aapt + " dump badging  " + '"'+ filename+'"';
        //    command=getadbOutput(cstring);

            QProcess run_command;
            run_command.setProcessChannelMode(QProcess::MergedChannels);
            run_command.start(cstring);
            run_command.waitForStarted();
            while(run_command.state() != QProcess::NotRunning)
            qApp->processEvents();
            QString command=run_command.readAll();



            mstringlist=command.split(QRegularExpression("[\t\n\r]"),Qt::SkipEmptyParts);


            for (QStringList::iterator it = mstringlist.begin();
                 it != mstringlist.end(); ++it)
            {
              QString item=*it;
              if (item.contains("package"))
              {
              QRegularExpression rx("(\\')");
              QStringList query = item.split(rx);
              QString packagename = query.at(1);
              logfile("package name query: "+packagename);
              QMessageBox::information(this, "",packagename);
              }
            }

 }




}


void MainWindow::on_actionSend_text_triggered()
{

 QString selectedDescription;
 if (!validateDeviceSelection(selectedDescription)) {
            return;
 }

 DeviceRecord device = queryDeviceRecord(selectedDescription);


 QString command;
 QString cstring;

 bool ok;
 QString text = QInputDialog::getText(this, tr("Text to Device"),
                                      tr("Send text:"), QLineEdit::Normal,
                                      QString(), &ok);
 if (ok && !text.isEmpty()) {

            text.replace(" ", "%s");
            cstring = "null -s " +device.daddr+ " shell input text " + text;

            command = getadbOutput(cstring);
            logfile(cstring);
            logfile(command);
 }

 return;

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

    m_xmlEditor->editXml(this, device, getadb(), isScoped(), scriptdir, busypath);
}



void MainWindow::on_actionScreen_Capture_triggered()
{

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
              QMessageBox::critical(this, "Error", "No device selected in table");
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
 DeviceRecord record;
 QString quotedDescription = "\"" + description + "\"";
 QSqlQuery query;

 QString sqlstatement = "SELECT Id, daddr, pulldir, xbmcpackage, data_root, buffermode, buffersize, "
                        "bufferfactor, description, filepath, port, isusb, disableroot, flag1, flag2, ostype, flag5 "
                        "FROM device WHERE description=" + quotedDescription;
 query.exec(sqlstatement);
 while (query.next()) {
               record.id = query.value("Id").toInt();
               record.daddr = query.value("daddr").toString();
               record.pulldir = query.value("pulldir").toString();
               record.xbmcpackage = query.value("xbmcpackage").toString();
               record.data_root = query.value("data_root").toString();
               record.buffermode = query.value("buffermode").toInt();
               record.buffersize = query.value("buffersize").toString();
               record.bufferfactor = query.value("bufferfactor").toString();
               record.description = query.value("description").toString();
               record.filepath = query.value("filepath").toString();
               record.port = query.value("port").toString();
               record.isusb = query.value("isusb").toBool();
               record.disableroot = query.value("disableroot").toBool();
               record.scoped = query.value("flag1").toBool();
               record.wsa = query.value("flag2").toBool();
               record.ostype = query.value("ostype").toString();
               record.scrcpyarg = query.value("flag5").toString(); // Map flag5 to scrcpyarg
 }
 if (query.lastError().isValid()) {
               logfile(sqlstatement);
               logfile("SqLite error:" + query.lastError().text());
               logfile("SqLite error code:" + query.lastError().nativeErrorCode());
 }

 return record;
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
               QMessageBox::critical(this, "", "No devices connected");
               return false;
 }

 // Get selected description from deviceTable
 int selectedRow = deviceTable->currentRow();
 if (selectedRow >= 0 && deviceTable->item(selectedRow, 0)) {
               selectedDescription = deviceTable->item(selectedRow, 0)->text();
 } else {
               QMessageBox::critical(this, "", "No device selected in table");
               return false;
 }

 // Check if the selected device is connected
 if (deviceTable->item(selectedRow, 2) &&
     deviceTable->item(selectedRow, 2)->text() != "Connected") {
               QMessageBox::critical(this, "", "Selected device is not connected");
               return false;
 }

 return true;
}








void MainWindow::screenCap()
{


      QString selectedDescription;
      if (!validateDeviceSelection(selectedDescription)) {
               return;
      }

      QJsonObject obj;
      QJsonDocument doc(obj);
      QFile file(databasedir + "adblink.json");
      file.open(QIODevice::ReadOnly);
      doc = QJsonDocument::fromJson(file.readAll());
      obj = doc.object();
      QString pulldir = obj["download"].toString();
      file.close();

      QString daddr;

      DeviceRecord device = queryDeviceRecord(selectedDescription);
      QString port = device.port.isEmpty() ? "5555" : device.port;

      if (device.isusb)
          daddr = device.daddr;
      else  daddr = device.daddr + ":" + port;

      QDateTime dateTime = QDateTime::currentDateTime();
      QString dtstr = dateTime.toString("yyyyMMdd_HHmmss");
      dtstr = dtstr + ".png";

      QString cstring = "null -s " + daddr + " shell screencap -p " + "/data/local/tmp/"+dtstr;


      QString command = getadbOutput(cstring);

      logfile(cstring);

      if (!command.isEmpty()) {
               logfile(command);
               QMessageBox::critical(this, "", "Screenshot failed: " + command);
               return;
      }

      if (!device.pulldir.isEmpty())
               pulldir = device.pulldir;




      cstring = "null -s " + device.daddr +  " pull "+ "/data/local/tmp/"+dtstr + " " + pulldir;

      command = getadbOutput(cstring);

      logfile(cstring);
      logfile(command);


      QString localFilePath = pulldir + "/" + dtstr;
      QFileInfo fileInfo(localFilePath);
      if (!fileInfo.exists()) {
               logfile("Error: Pulled file does not exist at " + localFilePath);
               QMessageBox::critical(this, "", "Failed to pull screenshot: File not found at " + localFilePath);
               return;
      }

      cstring =  cstring = "null -s " + device.daddr +   " shell rm " + "/data/local/tmp/"+dtstr;

      command = getadbOutput(cstring);

      logfile(cstring);
      logfile(command);

      QMessageBox::information(this, "", "Screenshot " + dtstr + " copied to " + pulldir);


}


//////////////////////////////////////

bool MainWindow::renameColumn(const QString& oldColumnName, const QString& newColumnName)
{



      QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
      db.setDatabaseName(dbstring);

      QSqlQuery query(db);
      query.prepare("PRAGMA table_info(device)");

      if (!query.exec()) {
               // qDebug() << "Failed to query table info for device table:" << query.lastError().text();
               return false;
      }

      bool oldColumnExists = false;
      bool newColumnExists = false;

      while (query.next()) {
               QString columnName = query.value("name").toString();
               if (columnName == oldColumnName) {
              oldColumnExists = true;
               }
               if (columnName == newColumnName) {
              newColumnExists = true;
               }
      }

      if (oldColumnExists && !newColumnExists) {
               QString sql = QString("ALTER TABLE device RENAME COLUMN %1 TO %2")
                                 .arg(oldColumnName, newColumnName);
               if (!query.exec(sql)) {
              // qDebug() << "Failed to rename column:" << query.lastError().text();
              return false;
               }
               // qDebug() << "Successfully renamed column" << oldColumnName << "to" << newColumnName << "in device table";
      } else if (!oldColumnExists) {
               // qDebug() << "Column" << oldColumnName << "does not exist in device table";
      } else if (newColumnExists) {
               // qDebug() << "Column" << newColumnName << "already exists in device table";
      }

      return true;
}


////////////////////////////////////////////////////////////

void MainWindow::serverlabel()
{


           QString cstring =  "null devices";
           QString command = getadbOutput(cstring);


}

/////////////////////////////////////////////////////////////////

bool MainWindow::validateIPAddress(const QString& ipAddress) {

           QString normalized = ipAddress.trimmed();


           QRegularExpression ipRegex(
               // IPv4 pattern
               "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}"
               "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"
               "|"

               "^[a-zA-Z0-9][a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9]?"
               "(\\.[a-zA-Z0-9][a-zA-Z0-9\\-]{0,61}[a-zA-Z0-9]?)*"
               "\\.[a-zA-Z0-9]{1,}$",
               QRegularExpression::CaseInsensitiveOption
               );

           return ipRegex.match(normalized).hasMatch();
}


////////////////////////////////////////

void MainWindow::delRecordButton_clicked()
{

   QString descrip;
   QString daddr;

   int selectedRow = deviceTable->currentRow();
   if (selectedRow >= 0 && deviceTable->item(selectedRow, 0)) {
                      descrip = deviceTable->item(selectedRow, 0)->text();
   } else {
                      QMessageBox::critical(this, "", "No device selected in table");
                      return;
   }

   if (!descrip.isEmpty())
   {
                      QMessageBox::StandardButton reply;
                      reply = QMessageBox::question(this, "", "Delete " + descrip + "?",
                                                    QMessageBox::Yes | QMessageBox::No);

                      if (reply == QMessageBox::No)
                      {
                          return;
                      }


                      deleteRecord(descrip);


                      selectedRow = deviceTable->currentRow();
                      daddr = deviceTable->item(selectedRow, 1)->text();



                      QString cstring = getadbpath() + " disconnect "+daddr;
                      QString command=getadbOutput(cstring);
                      logfile (command);
                      logfile("disconnect: "+daddr);

                      deviceTable->removeRow(selectedRow);
                      logfile(descrip + " is deleted");

   }
}


///////////////////////////////////////////////////


void MainWindow::createTables()
{
   logfile("Attempting to create tables for: " + dbstring);

   QSqlDatabase db = QSqlDatabase::database();
   if (!db.isOpen()) {
                      logfile("Error: Database not open");
                      return;
   }

   logfile("Database opened successfully: " + db.databaseName());

   // Check if the device table already exists
   QSqlQuery checkQuery(db);
   bool tableExists = false;
   if (checkQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='device';") && checkQuery.next()) {
                      tableExists = true;
                      //logfile("Device table already exists");
   } else if (checkQuery.lastError().isValid()) {
                      logfile("Error checking for device table: " + checkQuery.lastError().text());
                      return;
   }

   if (!tableExists) {
                      // Create the device table
                      QString sqlstatement = "CREATE TABLE IF NOT EXISTS device ("
                                             "Id INTEGER PRIMARY KEY, "
                                             "daddr TEXT, "
                                             "description TEXT NOT NULL UNIQUE, "
                                             "pulldir TEXT, "
                                             "xbmcpackage TEXT, "
                                             "data_root TEXT, "
                                             "buffermode INTEGER, "
                                             "buffersize TEXT, "
                                             "bufferfactor TEXT, "
                                             "filepath TEXT, "
                                             "port TEXT, "
                                             "isusb INTEGER, "
                                             "ostype TEXT, "
                                             "logfilename TEXT, "
                                             "disableroot INTEGER, "
                                             "flag1 TEXT, "
                                             "flag2 TEXT, "
                                             "flag3 TEXT, "
                                             "flag4 TEXT, "
                                             "flag5 TEXT)"; // scrcpy arg

                      QSqlQuery query(db);
                      if (!query.exec(sqlstatement)) {
                          logfile("SQL statement: " + sqlstatement);
                          logfile("SQLite error: " + query.lastError().text());
                           logfile("SQLite error code: " + query.lastError().nativeErrorCode());
                          logfile("Database file path: " + db.databaseName());
                      } else {
                          logfile("Successfully created new device table");
                          // Verify table creation
                          QSqlQuery verifyQuery(db);
                          if (verifyQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='device';") && verifyQuery.next()) {
               logfile("Device table creation confirmed");
                          } else {
               logfile("Error: Device table not found after creation attempt");
                          }
                      }
   }
}



//////////////////////////////////////////////

void MainWindow::deleteRecord(QString descrip)

{


   QString sqlstatement;

   QString quote = "\"";

   descrip = quote+descrip+quote;
   QSqlQuery query;


   sqlstatement= "DELETE FROM device WHERE description=" + descrip;
   query.exec(sqlstatement);



}

void MainWindow::on_Erase_adbLink_database_triggered()
{
   QMessageBox msgBox;
   msgBox.setTextFormat(Qt::PlainText);
   msgBox.setText("Initialize adblink?\nWARNING: This action will delete all device records and settings, then close and restart adblink. Are you sure you want to proceed?");
   msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
   msgBox.setIcon(QMessageBox::Critical);

   QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());

   if (reply == QMessageBox::Yes)
   {


                      QStringList connections = QSqlDatabase::connectionNames();
                      for (const QString& conn : connections) {
                          QSqlDatabase db = QSqlDatabase::database(conn, false);
                          if (db.isOpen()) {
               QSqlQuery query(db);
               query.clear();
               if (db.transaction()) {
              db.commit();
               }
               db.close();
                          }
                          QSqlDatabase::removeDatabase(conn);
                      }


                      QDir dir(databasedir);
                      if (dir.exists() && !dir.removeRecursively()) {

                  /* #ifdef Q_OS_WIN
                          QString command = QString("cmd.exe /C rmdir /S /Q \"%1\"").arg(databasedir.replace("/", "\\"));
                          QProcess::startDetached(command, QStringList());
                     #endif  */

                      }


                      QCoreApplication::quit();
                      QString program = QCoreApplication::applicationFilePath();
                      QStringList arguments = QCoreApplication::arguments();
                      QProcess::startDetached(program, arguments);
   }

}


//////////////////////////////////////////

void MainWindow::stopapp_clicked()
{
   QString selectedDescription;
   if (!validateDeviceSelection(selectedDescription)) {
                      return;
   }

   DeviceRecord device = queryDeviceRecord(selectedDescription);

   QString jsonPath = QDir(databasedir).filePath("adblink.json");
   QString stopapp = "org.xbmc.kodi"; // Default
   QJsonObject jsonObj;

   // Read existing JSON to preserve all fields
   QFile file(jsonPath);
   if (file.exists()) {
                      if (!file.open(QIODevice::ReadOnly)) {
                          QMessageBox::critical(this, "Error", "Cannot read adblink.json.");
                          return;
                      }
                      QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                      file.close();
                      if (doc.isObject()) {
                          jsonObj = doc.object();
                          if (jsonObj.contains("stopapp")) {
               stopapp = jsonObj["stopapp"].toString();
                          }
                      } else {
                          qWarning() << "Invalid JSON in adblink.json";
                      }
   }

   forcequitDialog dialog(false, stopapp, this);
   dialog.setWindowModality(Qt::WindowModal);
   if (dialog.exec() != QDialog::Accepted) {
                      return;
   }

   QString package = dialog.packagename();
   if (package.isEmpty()) {
                      QMessageBox::critical(this, "Error", "Invalid package name.");
                      return;
   }

   QString cstring = getadb() + " shell am force-stop " + package;
   QString command = getadbOutput(cstring);
   if (command.contains("error", Qt::CaseInsensitive)) {
                      qWarning() << "ADB command failed: " << command;
                      QMessageBox::warning(this, "Warning", "Failed to stop application.");
   }
   logfile(cstring);
   logfile(command);

   // Update JSON only if stopapp changed
   if (stopapp != package) {
                      jsonObj["stopapp"] = package;
                      if (!file.open(QIODevice::WriteOnly)) {
                          qWarning() << "Cannot write to adblink.json";
                          QMessageBox::critical(this, "Error", "Cannot save configuration.");
                          return;
                      }
                      file.write(QJsonDocument(jsonObj).toJson());
                      file.close();
   }
}



/////////////////////////////////////////

void MainWindow::startapp_clicked()
{
   QString selectedDescription;
   if (!validateDeviceSelection(selectedDescription)) {
                      return;
   }

   DeviceRecord device = queryDeviceRecord(selectedDescription);

   QString jsonPath = QDir(databasedir).filePath("adblink.json");
   QString startapp = "org.xbmc.kodi/org.xbmc.kodi.Splash"; // Default
   QJsonObject jsonObj;

   // Read existing JSON to preserve all fields
   QFile file(jsonPath);
   if (file.exists()) {
                      if (!file.open(QIODevice::ReadOnly)) {
                          QMessageBox::critical(this, "Error", "Cannot read adblink.json.");
                          return;
                      }
                      QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                      file.close();
                      if (doc.isObject()) {
                          jsonObj = doc.object();
                          if (jsonObj.contains("startapp")) {
               startapp = jsonObj["startapp"].toString();
                          }
                      } else {
                          qWarning() << "Invalid JSON in adblink.json";
                      }
   }

   forcequitDialog dialog(true, startapp, this);
   dialog.setWindowModality(Qt::WindowModal);
   if (dialog.exec() != QDialog::Accepted) {
                      return;
   }

   QString package = dialog.packagename();
   if (package.isEmpty()) {
                      QMessageBox::critical(this, "Error", "Invalid package name.");
                      return;
   }

   QString cstring = getadb() + " shell am start -n " + package;
   QString command = getadbOutput(cstring);
   if (command.contains("error", Qt::CaseInsensitive)) {
                      qWarning() << "ADB command failed: " << command;
                      QMessageBox::warning(this, "Warning", "Failed to start application.");
   }
   logfile(cstring);
   logfile(command);

   // Update JSON only if startapp changed
   if (startapp != package) {
                      jsonObj["startapp"] = package;
                      if (!file.open(QIODevice::WriteOnly)) {
                          qWarning() << "Cannot write to adblink.json";
                          QMessageBox::critical(this, "Error", "Cannot save configuration.");
                          return;
                      }
                      file.write(QJsonDocument(jsonObj).toJson());
                      file.close();
   }
}


////////////////////////////////

////////////////////////////////////

void MainWindow::displayOff()
{
    ::displayOff(getadb());
}




bool MainWindow::usbConnected(QString daddr)
{
    QString cstring =  "null devices";
    QString command = getadbOutput(cstring);
    return command.contains(daddr);
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
        // qDebug() << "Failed to write to adblink.json";
   }
}

///////////////////////////////////



QPushButton* MainWindow::setupDonateButton(QWidget* parent) {
   donateButton = new QPushButton(parent); // Assign to member variable
   QPixmap pix(":/assets/donatel.png");
   if (pix.isNull()) {
        // qDebug() << "Error: Failed to load :/assets/donatel.png";
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
   if (QMetaObject::checkConnectArgs(SIGNAL(clicked()), SLOT(on_donate_clicked()))) {
        connect(donateButton, &QPushButton::clicked, this, &MainWindow::on_donate_clicked);
   } else {
        // qDebug() << "Warning: on_donate_clicked slot not found";
   }

   QString donation = readDonationValue();
   setDonateButtonActive(donation != "jocala.com");

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
// Preserve connected devices
QSet<QString> connectedDeviceIds;
for (int row = 0; row < table->rowCount(); ++row) {
        if (table->item(row, 2) &&
            table->item(row, 2)->text() == "Connected" &&
            table->item(row, 0)) {
            connectedDeviceIds.insert(table->item(row, 0)->data(Qt::UserRole).toString());
        }
}

// Reset table
table->clearContents();
table->setRowCount(0);
table->setColumnCount(3);
table->setHorizontalHeaderLabels(QStringList() << "Device" << "IP" << "Status");
table->verticalHeader()->setVisible(false);
table->setEditTriggers(QAbstractItemView::NoEditTriggers);
table->setShowGrid(true);
table->setSortingEnabled(false);
table->setSelectionMode(QAbstractItemView::SingleSelection);
table->setSelectionBehavior(QAbstractItemView::SelectRows);

// Set font size based on windowSizeSelector
QFont tableFont = table->font();
switch (windowSizeSelector) {
case 0: tableFont.setPixelSize(sfontsize); break;
case 1: tableFont.setPixelSize(mfontsize); break;
case 2: tableFont.setPixelSize(lfontsize); break;
default: tableFont.setPixelSize(sfontsize); break;
}
table->setFont(tableFont);

// Populate table from database
QString sqlstatement = "SELECT id, description, daddr, isusb FROM device";
QSqlQuery query;
if (!query.exec(sqlstatement)) {
        logfile("Query failed: " + query.lastError().text());
        return;
}

int row = 0;
while (query.next()) {
        table->insertRow(row);

        QString deviceId = query.value(0).toString();
        QString description = query.value(1).toString();

        QTableWidgetItem* descItem = new QTableWidgetItem(description);
        descItem->setData(Qt::UserRole, deviceId);
        descItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        table->setItem(row, 0, descItem);

        bool isUsb = query.value(3).toBool();
        QString ip = isUsb ? "USB" : (query.value(2).toString().isEmpty() ? "N/A" : query.value(2).toString());
        table->setItem(row, 1, new IpTableWidgetItem(ip));

        QString status;
        if (isUsb)
            status = usbConnected(query.value(2).toString()) ? "Connected" : "Disconnected";
        else
            status = connectedDeviceIds.contains(deviceId) ? "Connected" : "Disconnected";

        table->setItem(row, 2, new QTableWidgetItem(status));
        row++;
}

// Adjust column widths
int tableWidth;
switch (windowSizeSelector) {
case 0: tableWidth = sMainWindowSize.width() * 0.63; break;
case 1: tableWidth = mMainWindowSize.width() * 0.63; break;
case 2: tableWidth = lMainWindowSize.width() * 0.55; break;
default: tableWidth = sMainWindowSize.width() * 0.63; break;
}
int colWidth = tableWidth / 3;
for (int i = 0; i < 3; ++i) table->setColumnWidth(i, colWidth);

// Make sure row heights match font
table->resizeRowsToContents();

// Let QTableWidget manage scrollbars automatically
table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

// Restore saved sort
QSettings settings("YourCompany", "YourApp");
int sortColumn = settings.value("DeviceTableSortColumn", 0).toInt();
Qt::SortOrder sortOrder = static_cast<Qt::SortOrder>(settings.value("DeviceTableSortOrder", Qt::AscendingOrder).toInt());
table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
table->setSortingEnabled(true);
table->sortItems(sortColumn, sortOrder);

// Update geometry
table->updateGeometry();
table->viewport()->update();
table->update();
if (centralWidget) {
        centralWidget->updateGeometry();
        centralWidget->update();
}

// Persist header click sort
disconnect(table->horizontalHeader(), &QHeaderView::sectionClicked, nullptr, nullptr);
connect(table->horizontalHeader(), &QHeaderView::sectionClicked, this, [table](int logicalIndex) {
    QSettings settings("YourCompany", "YourApp");
    settings.setValue("DeviceTableSortColumn", logicalIndex);
    settings.setValue("DeviceTableSortOrder", table->horizontalHeader()->sortIndicatorOrder());
});
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
   // qDebug() << "Failed to write to adblink.json";
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


 menuBar = new QMenuBar(this);
 setMenuBar(menuBar);
// menuBar->setNativeMenuBar(false);


  // File Menu
 menuFile = new QMenu("File", this);
 actionPreferences = new QAction("Preferences", this);
 actionView_adbLink_Log = new QAction("View adblink Log", this);
 actionQuit = new QAction("Quit", this);
 menuFile->addAction(actionPreferences);
 menuFile->addAction(actionView_adbLink_Log);
 menuFile->addSeparator();
 menuFile->addAction(actionQuit);

 // Kodi Menu
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

 // Utility Menu
 menuUtility = new QMenu("Utility", this);
 actionSwitch_View = new QAction("Switch View", this);
 actionReiinstall_Busybox = new QAction("Reinstall Busybox", this);
 infoArchitecture2 = new QAction("System information", this);
 actionOculus = new QAction("Oculus Headset", this);
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
 menuUtility->addAction(actionOculus);

 menuUtility->addAction(Erase_adbLink_database);
 menuUtility->addAction(actionSend_text);
 menuUtility->addAction(actionGet_UID_from_APK_file);
 menuUtility->addAction(actionReload_devices);
 menuUtility->addAction(View_Changelog);
 menuUtility->addAction(actionWireless_ADBD);
 menuUtility->addAction(actionReboot);



 // About Menu
 menuAbout = new QMenu("About", this);
 actionAbout = new QAction("About", this);
 menuAbout->addAction(actionAbout);

 // Help Menu
 menuHelp = new QMenu("Help", this);
 actionHelp = new QAction("Help", this);
 menuHelp->addAction(actionHelp);

 // Add menus to menubar
 menuBar->addMenu(menuFile);
 menuBar->addMenu(menuKodi);
 menuBar->addMenu(menuUtility);
 menuBar->addMenu(menuAbout);
 menuBar->addMenu(menuHelp);

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

 connect(actionReiinstall_Busybox, &QAction::triggered, this, [this](bool checked) {
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
 connect(actionOculus,               &QAction::triggered, this, &MainWindow::on_actionOculus_VR_triggered);
 connect(actionHelp,                 &QAction::triggered, this, &MainWindow::on_actionHelp_triggered);
}
