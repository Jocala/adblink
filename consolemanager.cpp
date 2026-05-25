#include "consolemanager.h"
#include "logfile.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QProcess>
#include <QTextStream>

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
