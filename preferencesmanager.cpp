#include "preferencesmanager.h"
#include "adbprefdialog.h"
#include "logfile.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

PreferencesManager::PreferencesManager(const QString &configPath, int os)
    : m_configPath(configPath), m_os(os)
{
}

bool PreferencesManager::exec(QWidget *parent)
{
    adbprefDialog dialog(parent);
    dialog.setWindowModality(Qt::WindowModal);

    QJsonObject obj;
    QJsonDocument doc(obj);
    QFile file(m_configPath);
    (void)file.open(QIODevice::ReadOnly);
    doc = QJsonDocument::fromJson(file.readAll());
    obj = doc.object();

    QString download = obj["download"].toString();
    int mcheck = obj["dropdown"].toInt();
    QString install = obj["install"].toString();
    QString backup = obj["backup"].toString();
    QString localadb = obj["localadb"].toString();

    bool checkversion = doc.object()["checkversion"].toBool();
    bool scrcpy = doc.object()["scrcpy"].toBool();
    bool startview = doc.object()["startview"].toBool();
    bool bypassdisconnect = obj.contains("bypassdisconnect") ? obj["bypassdisconnect"].toBool() : false;
    int defaultwindow = doc.object()["defaultwindow"].toInt();

    int fmfont = doc.object()["fmfont"].toInt();
    int lgfont = doc.object()["lgfont"].toInt();
    int smfont = doc.object()["smfont"].toInt();
    int mdfont = doc.object()["mdfont"].toInt();

    file.close();

    dialog.setversioncheck(checkversion);
    dialog.setscrcpyargs(scrcpy);
    dialog.setstartview(startview);
    dialog.setbypassdisconnect(bypassdisconnect);
    dialog.setdefaultwindow(defaultwindow);
    dialog.setlinterm(mcheck);
    dialog.setmacterm(mcheck);
    dialog.setdownloaddir(download);
    dialog.setlocaladb(localadb);
    dialog.setinstalldir(install);
    dialog.setbackupdir(backup);
    dialog.setfmfont(fmfont);
    dialog.setlgfont(lgfont);
    dialog.setmdfont(mdfont);
    dialog.setsmfont(smfont);

    dialog.setModal(true);

    if (dialog.exec() != QDialog::Accepted)
        return false;

    if (m_os == 0)
        obj["dropdown"] = dialog.linterm();
    if (m_os == 2)
        obj["dropdown"] = dialog.macterm();

    obj["checkversion"] = dialog.versioncheck();
    obj["scrcpy"] = dialog.scrcpyargs();
    obj["startview"] = dialog.startview();
    obj["bypassdisconnect"] = dialog.bypassdisconnect();
    obj["defaultwindow"] = dialog.defaultwindow();
    obj["download"] = dialog.downloaddir();
    obj["install"] = dialog.installdir();
    obj["backup"] = dialog.backupdir();
    obj["localadb"] = dialog.localadb();
    obj["fmfont"] = dialog.fmfont();
    obj["lgfont"] = dialog.lgfont();
    obj["mdfont"] = dialog.mdfont();
    obj["smfont"] = dialog.smfont();

    QJsonDocument writeDoc(obj);
    QFile writeFile(m_configPath);
    (void)writeFile.open(QIODevice::WriteOnly);
    writeFile.write(writeDoc.toJson());
    writeFile.close();

    m_result = {
        dialog.lgfont(),
        dialog.mdfont(),
        dialog.smfont(),
        dialog.fmfont()
    };

    return true;
}
