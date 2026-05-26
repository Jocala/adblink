#include "consolemanager.h"
#include "adbutils.h"
#include "getlocaladb.h"
#include "logfile.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>

#include "scpdialog.h"

ConsoleManager::ConsoleManager(QObject *parent)
    : QObject(parent)
    , m_os(detectOs())
{
}

ConsoleManager::OSType ConsoleManager::detectOs()
{
#ifdef Q_OS_LINUX
    return Linux;
#elif defined(Q_OS_WIN)
    return Windows;
#elif defined(Q_OS_MAC)
    return macOS;
#else
    return Linux;
#endif
}

QString ConsoleManager::adbShellScript(const QString &scriptDir, const QString &serial, const QString &adbfilesDir) const
{
    if (m_os == Windows) {
        QString path = scriptDir + QStringLiteral("/shell.bat");
        QFile file(path);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            logfile(QStringLiteral("error creating shell.bat!"));
            return QString();
        }
        QTextStream out(&file);
        out << QStringLiteral("echo off") << Qt::endl;
        out << QStringLiteral("set PATH=") + adbfilesDir + QStringLiteral(";%PATH%") << Qt::endl;
        out << QStringLiteral("adb.exe ") + serial + QStringLiteral(" shell -t \"export PATH=\\\"$PATH:/data/local/tmp/adblink\\\"; export PS1=\\\"$HOSTNAME:$PWD\\\\$\\\"; sh -i\"") << Qt::endl;
        file.flush();
        file.close();
        logfile(QStringLiteral("shell.bat created"));
        return path;
    }

    QString path = scriptDir + QStringLiteral("/shell.sh");
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        logfile(QStringLiteral("error creating shell.sh!"));
        return QString();
    }
    QTextStream out(&file);
    out << QStringLiteral("#!/bin/sh") << Qt::endl;
    out << QStringLiteral("export PATH=\"") + adbfilesDir + QStringLiteral("\":$PATH") << Qt::endl;
    out << QStringLiteral("adb ") + serial + QStringLiteral(" shell -t \"export PATH=\\\"$PATH:/data/local/tmp/adblink\\\"; export PS1=\\\"$HOSTNAME:$PWD\\\\$\\\"; sh -i\"") << Qt::endl;
    file.flush();
    file.close();

    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
                        QFileDevice::ReadGroup | QFileDevice::ExeGroup |
                        QFileDevice::ReadOther | QFileDevice::ExeOther);
    return path;
}

QString ConsoleManager::consoleScript(const QString &scriptDir, const QString &serial, const QString &adbfilesDir) const
{
    if (m_os == Windows) {
        QString path = scriptDir + QStringLiteral("/cpath.bat");
        QFile file(path);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            logfile(QStringLiteral("error creating cpath.bat!"));
            return QString();
        }
        QTextStream out(&file);
        out << QStringLiteral("echo off") << Qt::endl;
        out << QStringLiteral("set PATH=") + adbfilesDir + QStringLiteral(";%PATH%") << Qt::endl;
        file.flush();
        file.close();
        return path;
    }

    QString path = scriptDir + QStringLiteral("/cpath.sh");
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        logfile(QStringLiteral("error creating cpath.sh!"));
        return QString();
    }
    QTextStream out(&file);
    out << QStringLiteral("#!/bin/sh") << Qt::endl;
    out << QStringLiteral("export PATH=\"") + adbfilesDir + QStringLiteral("\":$PATH") << Qt::endl;
    out << QStringLiteral("/bin/sh") << Qt::endl;
    file.flush();
    file.close();

    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
                        QFileDevice::ReadGroup | QFileDevice::ExeGroup |
                        QFileDevice::ReadOther | QFileDevice::ExeOther);
    return path;
}

QString ConsoleManager::scrcpyScript(const QString &scriptDir, const QString &adbfilesDir, const QString &scrcpyDir) const
{
    if (m_os == Windows) {
        QString path = scriptDir + QStringLiteral("/scrcpy.bat");
        QFile file(path);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            logfile(QStringLiteral("error creating scrcpy.bat!"));
            return QString();
        }
        QTextStream out(&file);
        out << QStringLiteral("echo off") << Qt::endl;
        out << QStringLiteral("set PATH=") + adbfilesDir + QStringLiteral(";") + scrcpyDir + QStringLiteral(";%PATH%") << Qt::endl;
        file.flush();
        file.close();
        return path;
    }

    QString path = scriptDir + QStringLiteral("/scrcpy.sh");
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        logfile(QStringLiteral("error creating scrcpy.sh!"));
        return QString();
    }
    QTextStream out(&file);
    out << QStringLiteral("#!/bin/sh") << Qt::endl;
    out << QStringLiteral("export PATH=\"") + adbfilesDir + QStringLiteral("\":\"") + scrcpyDir + QStringLiteral("\":$PATH") << Qt::endl;
    file.flush();
    file.close();

    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
                        QFileDevice::ReadGroup | QFileDevice::ExeGroup |
                        QFileDevice::ReadOther | QFileDevice::ExeOther);
    return path;
}

void ConsoleManager::launchTerminal(const QString &scriptPath, int terminalChoice) const
{
    QString cmd = terminalCommand(scriptPath, terminalChoice);
    if (!cmd.isEmpty())
        QProcess::startDetached(cmd);
}

QString ConsoleManager::terminalCommand(const QString &scriptPath, int terminalChoice) const
{
    if (m_os == Windows)
        return QStringLiteral("cmd.exe /c start \"\" ") + scriptPath;

    if (m_os == macOS) {
        switch (terminalChoice) {
        case 0:  return QStringLiteral("open -a Terminal.app ") + scriptPath;
        case 1:  return QStringLiteral("open -a iTerm.app ") + scriptPath;
        default: return QStringLiteral("open -a Terminal.app ") + scriptPath;
        }
    }

    // Linux
    switch (terminalChoice) {
    case 0:  return QStringLiteral("/usr/bin/gnome-terminal -- ") + scriptPath;
    case 1:  return QStringLiteral("/usr/bin/xfce4-terminal -e ") + scriptPath;
    case 2:  return QStringLiteral("/usr/bin/konsole -e ") + scriptPath;
    default: return QStringLiteral("/usr/bin/gnome-terminal -- ") + scriptPath;
    }
}

void ConsoleManager::openConsole(const QString &scriptDir, const QString &appHome,
                                 const QString &dataBaseDir) const
{
    logfile("adb: " + getadbpath());
    logfile("detaching console process");

    QJsonObject obj;
    QJsonDocument doc(obj);
    QFile file(dataBaseDir + "adblink.json");
    file.open(QIODevice::ReadOnly);
    doc = QJsonDocument::fromJson(file.readAll());
    obj = doc.object();
    int mcheck = obj["dropdown"].toInt();

    QString cstring;
    QString adbfilesDir = QCoreApplication::applicationDirPath() + "/adbfiles";

    if (m_os == Windows)
    {
        QString commstr = scriptDir + "/cpath.bat";
        QFile file(commstr);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
            logfile("error creating cpath.bat!");
            QMessageBox::critical(nullptr, "", "Error creating bat file!");
            return;
        }

        QTextStream out(&file);
        out << "echo off" << Qt::endl;
        if (getlocaladb().isEmpty())
            out << "set PATH=" + adbfilesDir + ";%PATH%" << Qt::endl;
        file.flush();
        file.close();

        QString outputString;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            outputString = in.readAll();
            file.close();
        }
        logfile("cpath.bat:");
        logfile(outputString);

        QProcess::startDetached("cmd.exe", QStringList() << "/c" << "start" << "" << commstr);
        return;
    }

    // macOS / Linux
    {
        QFileDevice::Permissions permissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
                                               QFileDevice::ReadGroup | QFileDevice::ExeGroup |
                                               QFileDevice::ReadOther | QFileDevice::ExeOther;

        QString commstr = scriptDir + "cpath.sh";
        QFile file(commstr);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
            logfile("error creating cpath!");
            QMessageBox::critical(nullptr, "", "Error creating sh file!");
            return;
        }

        QTextStream out(&file);
        out << "#!/bin/sh" << Qt::endl;
        if (getlocaladb().isEmpty())
            out << "export PATH=\"" + adbfilesDir + "\":$PATH" << Qt::endl;
        out << "/bin/sh" << Qt::endl;
        file.flush();
        file.close();

        QString outputString;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            outputString = in.readAll();
            file.close();
        }
        logfile("cpath.sh:");
        logfile(outputString);

        if (!file.setPermissions(permissions)) {
            logfile("Failed to set permissions for " + commstr);
            return;
        }
    }

    // Launch terminal
    if (m_os == macOS)
    {
        switch (mcheck)
        {
        case 0:
            cstring = "open -a Terminal.app " + scriptDir + "cpath.sh";
            break;
        case 1:
            cstring = "open -a iTerm.app " + scriptDir + "cpath.sh";
            break;
        default:
            cstring = "open -a Terminal.app " + scriptDir + "cpath.sh";
        }
    }
    else if (m_os == Linux)
    {
        switch (mcheck)
        {
        case 0:
            cstring = "/usr/bin/gnome-terminal --working-directory=" + appHome + " -x " + scriptDir + "cpath.sh";
            break;
        case 1:
            cstring = "/usr/bin/xfce4-terminal --working-directory=" + appHome + " -x " + scriptDir + "cpath.sh";
            break;
        case 2:
            cstring = "/usr/bin/konsole --workdir=" + appHome + " -e " + scriptDir + "cpath.sh";
            break;
        default:
            cstring = "/usr/bin/gnome-terminal --working-directory=" + appHome + " -x " + scriptDir + "cpath.sh";
        }
    }

    QProcess::startDetached(cstring);
}

void ConsoleManager::openAdbShell(const QString &daddr, const QString &scriptDir,
                                   const QString &appHome, const QString &dataBaseDir) const
{
    logfile("adb: " + getadbpath());
    logfile("detaching console process");

    QJsonObject obj;
    QJsonDocument doc(obj);
    QFile file(dataBaseDir + "adblink.json");
    file.open(QIODevice::ReadOnly);
    doc = QJsonDocument::fromJson(file.readAll());
    obj = doc.object();
    int mcheck = obj["dropdown"].toInt();

    QString cstring;

    if (m_os == Windows)
    {
        QString commstr = scriptDir + "/shell.bat";
        QFile file(commstr);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
            logfile("error creating shell.bat!");
            QMessageBox::critical(nullptr, "", "Error creating bat file!");
            return;
        }

        QTextStream out(&file);
        out << "echo off" << Qt::endl;
        QString adbfilesDir = QCoreApplication::applicationDirPath() + "/adbfiles";
        out << "set PATH=" + adbfilesDir + ";%PATH%" << Qt::endl;
        out << "adb.exe -s " + daddr + " shell -t \"export PATH=\\\"$PATH:/data/local/tmp/adblink\\\"; export PS1=\\\"$HOSTNAME:$PWD\\\\$\\\"; sh -i\"" << Qt::endl;
        file.flush();
        file.close();

        QString outputString;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            outputString = in.readAll();
            file.close();
        }
        logfile("shell.bat:");
        logfile(outputString);

        QProcess::startDetached("cmd.exe", QStringList() << "/c" << "start" << "" << commstr);
        return;
    }

    // macOS / Linux
    {
        QString commstr = scriptDir + "console.sh";
        QFile::remove(commstr);
        QFile file(commstr);
        if (!file.open(QFile::WriteOnly))
        {
            logfile("error creating console.sh!");
            QMessageBox::critical(nullptr, "", "Error creating command file!");
            return;
        }

        QTextStream out(&file);
        out << "#!/bin/sh" << Qt::endl;
        QString adbPath = getadbpath();
        out << "\"" + adbPath + "\" -s " + daddr + " shell -t \"export PATH=\\$PATH:/data/local/tmp/adblink; sh -i\"" << Qt::endl;
        out << cstring << Qt::endl;
        file.flush();
        file.close();

        QString outputString;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            outputString = in.readAll();
            file.close();
        }
        logfile("console.sh:");
        logfile(outputString);

        QString shelldir = '"' + scriptDir + "console.sh" + '"';

        QFileDevice::Permissions permissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
                                               QFileDevice::ReadGroup | QFileDevice::ExeGroup |
                                               QFileDevice::ReadOther | QFileDevice::ExeOther;

        if (!file.setPermissions(permissions)) {
            logfile("Failed to set permissions for " + shelldir);
            return;
        }

        if (m_os == Linux)
        {
            switch (mcheck)
            {
            case 0:
                cstring = "/usr/bin/gnome-terminal --working-directory=" + appHome + " -x " + shelldir;
                break;
            case 1:
                cstring = "/usr/bin/xfce4-terminal --working-directory=" + appHome + " -x " + shelldir;
                break;
            case 2:
                cstring = "/usr/bin/konsole --workdir=" + appHome + " -e " + shelldir;
                break;
            default:
                cstring = "/usr/bin/gnome-terminal --working-directory=" + appHome + " -x " + shelldir;
            }
        }

        if (m_os == macOS)
        {
            switch (mcheck)
            {
            case 0:
                cstring = "open -a Terminal.app " + shelldir;
                break;
            case 1:
                cstring = "open -a iTerm.app " + shelldir;
                break;
            default:
                cstring = "open -a Terminal.app " + shelldir;
            }
        }
    }

    QProcess::startDetached(cstring);
}

void ConsoleManager::openScrcpy(QWidget *parent, const QString &daddr,
                                 const QString &scriptDir, const QString &appHome,
                                 const QString &dataBaseDir, const QString &adbfilesDir,
                                 const QString &scrcpyDir, const QString &scrcpyarg) const
{
    logfile("starting scrcpy function");

    QJsonObject obj;
    QJsonDocument doc(obj);
    QFile file(dataBaseDir + "adblink.json");
    file.open(QIODevice::ReadOnly);
    doc = QJsonDocument::fromJson(file.readAll());
    obj = doc.object();
    bool scrcpy = doc.object()["scrcpy"].toBool();
    int mcheck = obj["dropdown"].toInt();

    QString scrcpybat = scriptDir + "scrcpy.bat";
    QString scrcpytxt = scriptDir + "scrcpy.txt";
    QString line;

    if (QFileInfo(scrcpytxt).exists())
    {
        QFile argfile(scrcpytxt);
        if (!argfile.open(QIODevice::ReadOnly)) {
            QMessageBox::information(nullptr, "error", argfile.errorString());
        }

        QTextStream in(&argfile);
        while (!in.atEnd()) {
            line = in.readLine();
        }
        argfile.close();
    }

    logfile("detaching scrcpy console process");

    if (!scrcpyarg.isEmpty())
        line = scrcpyarg;

    QString argval;
    if (scrcpy)
    {
        scpDialog dialog;
        dialog.setArgs(line);

        if (dialog.exec() == QDialog::Accepted)
            argval = dialog.scpArgs();
        else
            return;
    }

    QString sernum = " -s " + daddr + " ";
    QString cstring;

    if (m_os == Windows)
    {
        QString commstr2 = scriptDir + "/scrcpy.txt";
        QFile file2(commstr2);
        if (!file2.open(QFile::WriteOnly | QFile::Text))
        {
            logfile("error creating scrcpy.txt!");
            QMessageBox::critical(parent, "", "Error saving arguments!");
            return;
        }
        QTextStream out2(&file2);
        out2 << argval << Qt::endl;
        file2.flush();
        file2.close();

        QString commstr = scriptDir + "/scrcpy.bat";
        QFile batFile(commstr);
        if (!batFile.open(QFile::WriteOnly | QFile::Text))
        {
            logfile("error creating scrcpy.bat!");
            QMessageBox::critical(parent, "", "Error creating bat file!");
            return;
        }
        QTextStream out(&batFile);
        out << "set PATH=" + adbfilesDir + ";" + scrcpyDir + ";%PATH%" << Qt::endl;
        out << "scrcpy.exe -s " + daddr + " " + argval << Qt::endl;
        batFile.flush();
        batFile.close();

        QProcess::startDetached("cmd.exe", QStringList() << "/c" << "start" << "" << commstr);
        return;
    }

    // macOS / Linux
    {
        QString commstr2 = scriptDir + "/scrcpy.txt";
        QFile file2(commstr2);
        if (!file2.open(QFile::WriteOnly | QFile::Text))
        {
            logfile("error saving arguments!");
            QMessageBox::critical(parent, "", "Error saving arguments!");
            return;
        }
        QTextStream out2(&file2);
        out2 << argval << Qt::endl;
        file2.flush();
        file2.close();

        QString commstr = scriptDir + "/scrcpy.sh";
        QFile shFile(commstr);
        if (!shFile.open(QFile::WriteOnly | QFile::Text))
        {
            logfile("error creating cpath!");
            QMessageBox::critical(parent, "", "Error creating script!");
            return;
        }

        QTextStream out(&shFile);
        out << "#!/bin/sh" << Qt::endl;
        out << "scrcpy -s " + daddr + " " + argval << Qt::endl;
        shFile.flush();
        shFile.close();

        QFileDevice::Permissions permissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
                                               QFileDevice::ReadGroup | QFileDevice::ExeGroup |
                                               QFileDevice::ReadOther | QFileDevice::ExeOther;

        if (!shFile.setPermissions(permissions)) {
            logfile("Failed to set permissions for " + commstr);
            return;
        }
    }

    if (m_os == macOS)
    {
        switch (mcheck)
        {
        case 0:
            cstring = "open -a Terminal.app " + scriptDir + "/scrcpy.sh";
            break;
        case 1:
            cstring = "open -a iTerm.app " + scriptDir + "/scrcpy.sh";
            break;
        default:
            cstring = "open -a Terminal.app " + scriptDir + "/scrcpy.sh";
        }
    }
    else if (m_os == Linux)
    {
        switch (mcheck)
        {
        case 0:
            cstring = "gnome-terminal --working-directory=" + QDir::fromNativeSeparators(appHome) + " -x " + scriptDir + "/scrcpy.sh";
            break;
        case 1:
            cstring = "xfce4-terminal --working-directory=" + QDir::fromNativeSeparators(appHome) + " -x " + scriptDir + "/scrcpy.sh";
            break;
        case 2:
            cstring = "konsole --workdir=" + QDir::fromNativeSeparators(appHome) + " -e " + scriptDir + "/scrcpy.sh";
            break;
        default:
            cstring = "gnome-terminal --working-directory=" + QDir::fromNativeSeparators(appHome) + " -x " + scriptDir + "/scrcpy.sh";
        }
    }

    QProcess::startDetached(cstring);
}
