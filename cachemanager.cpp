#include "cachemanager.h"
#include "cachedialog.h"
#include "adbutils.h"
#include "getadbdata.h"
#include "logfile.h"

#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>

bool CacheManager::configureCache(QWidget *parent, const DeviceRecord &device,
                                   const QString &adbPrefix, bool scoped,
                                   const QString &scriptDir)
{
    QString buffersize;
    QString bufferfactor;
    int buffermode = 1;

    bool oldxml = false;

    QString line1, line2, line3, line4, line5, line6, line7;

    QString mcpath;
    QString xpath;
    QString cstring;

    mcpath = resolveKodiPath(adbPrefix, device.data_root, device.xbmcpackage, scoped);

    xpath = mcpath + "/userdata/";

    cstring = adbPrefix + " shell ls " + mcpath;
    QString command = getadbOutput(cstring);

    if (command.contains("No such file or directory")) {
        QMessageBox msgBox(parent);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(QString());
        msgBox.setText(QStringLiteral("Cache folder missing. Run Kodi to create it."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.exec();
        return false;
    }

    cacheDialog dialog(parent);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setxmlcheck(false);
    dialog.setcbuffermode(buffermode);
    dialog.setcbuffersize(buffersize);
    dialog.setcbufferfactor(bufferfactor);
    dialog.setModal(true);

    if (dialog.exec() != QDialog::Accepted)
        return false;

    buffermode = dialog.cbuffermode();
    buffersize = dialog.cbuffersize();
    bufferfactor = dialog.cbufferfactor();
    oldxml = dialog.xmlcheck();

    QString str1 = QString::number(buffermode - 1);

    if (oldxml) {
        line1 = "<advancedsettings>";
        line2 = "  <network>";
        line3 = "    <buffermode>" + str1 + "</buffermode>";
        line4 = "    <cachemembuffersize>" + buffersize + "</cachemembuffersize>";
        line5 = "    <readbufferfactor>" + bufferfactor + "</readbufferfactor>";
        line6 = "  </network>";
        line7 = "</advancedsettings>";
    } else {
        line1 = "<advancedsettings>";
        line2 = "  <cache>";
        line3 = "    <buffermode>" + str1 + "</buffermode>";
        line4 = "    <memorysize>" + buffersize + "</memorysize>";
        line5 = "    <readfactor>" + bufferfactor + "</readfactor>";
        line6 = "  </cache>";
        line7 = "</advancedsettings>";
    }

    if (dialog.returncval1() == 2) {
        logfile("write advancedsettings.xml");

        cstring = adbPrefix + " shell ls " + mcpath;
        command = getadbOutput(cstring);
        if (command.contains("No such file or directory")) {
            QMessageBox msgBox(parent);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Destination path missing"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            return false;
        }

        QString filename1 = "advancedsettings.xml";
        QString filename2 = scriptDir + filename1;

        cstring = adbPrefix + " shell ls " + xpath + filename1;
        command = getadbOutput(cstring);
        logfile(command);

        if (!command.contains("No such file or directory")) {
            logfile("advancedsettings.xml exists");

            QMessageBox msgBox(parent);
            msgBox.setWindowTitle(QStringLiteral("XML"));
            msgBox.setText(QStringLiteral("advancedsettings.xml exists. Overwrite?\n(will backup original)"));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setWindowModality(Qt::WindowModal);
            QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());
            if (reply == QMessageBox::No) {
                logfile("abort xml write");
                return false;
            } else {
                logfile("continue xml write");
                logfile("backup advancedsettings.xml");

                cstring = adbPrefix + " shell cp " + xpath + filename1 + " " + xpath + filename1 + ".old";
                command = getadbOutput(cstring);
            }
        }

        QFile file(filename2);
        if (!file.open(QFile::WriteOnly)) {
            logfile("error creating advancedsettings.xml.");
            QMessageBox msgBox(parent);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("Unknown error creating xml file!"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            return false;
        }

        QTextStream out(&file);
        out << line1 << Qt::endl;
        out << line2 << Qt::endl;
        out << line3 << Qt::endl;
        out << line4 << Qt::endl;
        out << line5 << Qt::endl;
        out << line6 << Qt::endl;
        out << line7 << Qt::endl;
        file.flush();
        file.close();

        cstring = adbPrefix + " push " + filename2 + " " + xpath + filename1;
        command = getadbOutput(cstring);
        logfile("PUSH " + command);

        if (!command.contains("bytes")) {
            logfile("error pushing xml script to device!");
            logfile(cstring);
            logfile(command);
            logfile("=============");
            QMessageBox msgBox(parent);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("See log: error pushing xml from PC to device"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
            return false;
        } else {
            QMessageBox msgBox(parent);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle(QString());
            msgBox.setText(QStringLiteral("advancedsettings.xml written"));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setWindowModality(Qt::WindowModal);
            msgBox.exec();
        }
    }

    return true;
}

CacheManager::CacheManager(QObject *parent)
    : QObject(parent)
{
}
