#include "logfile.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QtGlobal>
#include <QMutex>
#include <QDebug> // Added to fix QDebug incomplete type error

#ifdef Q_OS_WIN
const QString databasedir = QDir::homePath() + "/AppData/Roaming/.jocala/";
#elif defined(Q_OS_UNIX)
const QString databasedir = QDir::homePath() + "/.jocala/";
#endif

void logfile(const QString& line)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    QFile file(databasedir + "adblink.log");
    if (!file.open(QFile::WriteOnly | QFile::Text | QFile::Append)) {
        qWarning() << "Can't create logfile:" << databasedir + "adblink.log";
        return;
    }

    QTextStream out(&file);
    out << line << Qt::endl;
    file.close();
}
