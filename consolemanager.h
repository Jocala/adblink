#ifndef CONSOLEMANAGER_H
#define CONSOLEMANAGER_H

#include <QObject>
#include <QStringList>

class ConsoleManager : public QObject
{
    Q_OBJECT

public:
    enum OSType { Linux = 0, Windows = 1, macOS = 2 };

    explicit ConsoleManager(QObject *parent = nullptr);

    void setOsType(OSType os) { m_os = os; }
    OSType osType() const { return m_os; }

    QString adbShellScript(const QString &scriptDir, const QString &serial, const QString &adbfilesDir) const;
    QString consoleScript(const QString &scriptDir, const QString &serial, const QString &adbfilesDir) const;
    QString scrcpyScript(const QString &scriptDir, const QString &adbfilesDir, const QString &scrcpyDir) const;

    void launchTerminal(const QString &scriptPath, int terminalChoice) const;
    QString terminalCommand(const QString &scriptPath, int terminalChoice) const;
    void openConsole(const QString &scriptDir, const QString &appHome,
                     const QString &dataBaseDir) const;
    void openAdbShell(const QString &daddr, const QString &scriptDir,
                      const QString &appHome, const QString &dataBaseDir) const;
    void openScrcpy(QWidget *parent, const QString &daddr,
                    const QString &scriptDir, const QString &appHome,
                    const QString &dataBaseDir, const QString &adbfilesDir,
                    const QString &scrcpyDir, const QString &scrcpyarg) const;

    static OSType detectOs();

signals:
    void logMessage(const QString &msg) const;

private:
    OSType m_os;
};

#endif // CONSOLEMANAGER_H
