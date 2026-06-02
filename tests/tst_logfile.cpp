#include <QtTest>
#include "logfile.h"

#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

class TestLogFile : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_tmp;

private slots:
    void initTestCase()
    {
        QVERIFY(m_tmp.isValid());
    }

    void writeSingleLine()
    {
        QString logPath = m_tmp.path() + "/writetest.log";

        // logfile() writes to ~/.jocala/adblink.log — we can't redirect it,
        // but we CAN test rotateLogFile() which takes a directory parameter.
    }

    void rotateNoFiles()
    {
        QString dir = m_tmp.path() + "/rotate_empty/";
        QDir().mkpath(dir);
        // Should not crash when neither file exists
        rotateLogFile(dir);
        QVERIFY(!QFile::exists(dir + "adblink.old.log"));
        QVERIFY(!QFile::exists(dir + "adblink.log"));
    }

    void rotateWithOnlyLog()
    {
        QString dir = m_tmp.path() + "/rotate_logonly/";
        QDir().mkpath(dir);

        // Create adblink.log
        QFile log(dir + "adblink.log");
        QVERIFY(log.open(QFile::WriteOnly));
        QTextStream(&log) << "test log content" << Qt::endl;
        log.close();

        rotateLogFile(dir);

        QVERIFY(QFile::exists(dir + "adblink.old.log"));
        QVERIFY(!QFile::exists(dir + "adblink.log"));
    }

    void rotateWithBothFiles()
    {
        QString dir = m_tmp.path() + "/rotate_both/";
        QDir().mkpath(dir);

        // Create adblink.old.log (will be removed)
        QFile oldLog(dir + "adblink.old.log");
        QVERIFY(oldLog.open(QFile::WriteOnly));
        QTextStream(&oldLog) << "old content" << Qt::endl;
        oldLog.close();

        // Create adblink.log (will be renamed to adblink.old.log)
        QFile log(dir + "adblink.log");
        QVERIFY(log.open(QFile::WriteOnly));
        QTextStream(&log) << "newer content" << Qt::endl;
        log.close();

        rotateLogFile(dir);

        // Old file was removed, new file was renamed
        QVERIFY(QFile::exists(dir + "adblink.old.log"));
        QVERIFY(!QFile::exists(dir + "adblink.log"));

        // Verify content of the rotated file matches original adblink.log
        QFile rotated(dir + "adblink.old.log");
        QVERIFY(rotated.open(QFile::ReadOnly));
        QCOMPARE(QString::fromUtf8(rotated.readAll()).trimmed(),
                 QStringLiteral("newer content"));
    }
};

QTEST_MAIN(TestLogFile)
#include "tst_logfile.moc"
