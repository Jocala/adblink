#include "xmleditormanager.h"
#include "adbutils.h"
#include "editordialog.h"
#include "getadbdata.h"
#include "listfiledialog.h"
#include "logfile.h"

#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>

void XmlEditorManager::editXml(QWidget *parent, const DeviceRecord &device,
                                const QString &adbPrefix, bool scoped,
                                const QString &scriptDir, const QString &busyPath)
{
    QString tempfile1;
    QString tempfile2;
    QString xpath;
    QString fileName;
    QString cstring;
    QString command;

    QString mcpath = resolveKodiPath(adbPrefix, device.data_root, device.xbmcpackage, scoped);
    xpath = mcpath + "/userdata/";

    cstring = adbPrefix + " shell " + busyPath + "busybox find " + xpath + " -maxdepth 1 -name *.xml ";
    command = getadbOutput(cstring);

    QStringList filelist = command.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);

    if (command.isEmpty() || command.contains("No such file or directory")) {
        QMessageBox::critical(parent, "", "No files found");
        logfile(command);
        logfile("no files found!");
        return;
    }

    listfileDialog fdialog(parent);
    fdialog.setWindowModality(Qt::WindowModal);
    fdialog.setFilelist(filelist);
    fdialog.setDialogTitle("XML Files");

    if (fdialog.exec() != QDialog::Accepted)
        return;

    fileName = fdialog.return_fitem();

    if (fileName.isEmpty()) {
        QMessageBox::critical(parent, "", "No file selected");
        logfile("no file selected");
        return;
    }

    QString filename(fileName.mid(fileName.lastIndexOf("/") + 1, fileName.length()));

    cstring = adbPrefix + " pull " + '"' + fileName + '"' + " " + '"' + scriptDir + '"' + "/" + filename;
    command = getadbOutput(cstring);

    if (!command.contains("bytes")) {
        logfile("edit failed");
        logfile(command);
        QMessageBox::critical(parent, "", "Edit failed " + command);
        return;
    }

    QFile file1(scriptDir + filename);
    if (!file1.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString xmlfile = file1.readAll();
    editorDialog dialog;
    dialog.seteditor(xmlfile);
    dialog.setfilename(filename);

    tempfile2 = scriptDir + "/" + filename;
    tempfile1 = scriptDir + "/" + filename + ".bak";

    dialog.setModal(true);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parent, "Save", "Save " + fileName + "?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    xmlfile = dialog.xmlfile();

    QFile::copy(scriptDir + "/" + filename, scriptDir + filename + ".bak");
    QFile caFile(scriptDir + "/" + filename);
    (void)caFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream outStream(&caFile);
    outStream << xmlfile;
    caFile.close();

    cstring = adbPrefix + " push " + '"' + tempfile1 + '"' + " " + xpath;
    command = getadbOutput(cstring);
    logfile(command);

    if (!command.contains("bytes")) {
        QMessageBox::critical(parent, "", "Backup of " + filename + " failed. Edit abandoned.");
        logfile("Backup of " + filename + " failed");
        return;
    }

    cstring = adbPrefix + " push " + '"' + tempfile2 + '"' + " " + xpath;
    command = getadbOutput(cstring);
    logfile(command);

    if (!command.contains("bytes")) {
        QMessageBox::critical(parent, "", "Problem replacing " + filename + ". Edit abandoned.");
        logfile("Problem replacing " + filename + ". Edit abandoned.");
        return;
    }

    QFile file2(tempfile1);
    file2.remove();

    QFile file3(tempfile2);
    file3.remove();
}

XmlEditorManager::XmlEditorManager(QObject *parent)
    : QObject(parent)
{
}
