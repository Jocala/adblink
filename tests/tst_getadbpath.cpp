#include <QtTest>
#include "getlocaladb.h"

#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

class TestGetLocalAdb : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_tmp;
    QString m_configDir;

    void writeJson(const QString &path, const QJsonObject &obj)
    {
        QFile file(path);
        QVERIFY2(file.open(QFile::WriteOnly),
                 qPrintable("Cannot write " + path));
        file.write(QJsonDocument(obj).toJson());
        file.close();
    }

private slots:
    void initTestCase()
    {
        QVERIFY(m_tmp.isValid());
        m_configDir = m_tmp.path() + "/config";
        QDir().mkpath(m_configDir);
    }

    void noJsonFileReturnsEmpty()
    {
        QCOMPARE(getlocaladb(m_configDir), QString());
    }

    void noLocalAdbKeyReturnsEmpty()
    {
        QJsonObject obj;
        obj["otherkey"] = "value";
        writeJson(m_configDir + "/adblink.json", obj);

        QCOMPARE(getlocaladb(m_configDir), QString());
    }

    void localAdbKeySetButNoBinary()
    {
        QJsonObject obj;
        obj["localadb"] = m_tmp.path() + "/nonexistent";
        writeJson(m_configDir + "/adblink.json", obj);

        // Returns empty because the binary doesn't exist at that path
        QCOMPARE(getlocaladb(m_configDir), QString());
    }

    void validLocalAdbFound()
    {
        // Create a fake ADB binary at the expected path
        QString fakeAdbDir = m_tmp.path() + "/myadb";
        QDir().mkpath(fakeAdbDir);
        QString fakeAdbPath = fakeAdbDir + "/adb";
#ifdef Q_OS_WIN
        fakeAdbPath += ".exe";
#endif
        QFile fakeAdb(fakeAdbPath);
        QVERIFY(fakeAdb.open(QFile::WriteOnly));
        fakeAdb.write("fake");
        fakeAdb.close();

        QJsonObject obj;
        obj["localadb"] = fakeAdbDir;
        writeJson(m_configDir + "/adblink.json", obj);

        QCOMPARE(getlocaladb(m_configDir),
                 QDir::cleanPath(fakeAdbPath));
    }

    void emptyLocalAdbKeyReturnsEmpty()
    {
        QJsonObject obj;
        obj["localadb"] = "";
        writeJson(m_configDir + "/adblink.json", obj);

        QCOMPARE(getlocaladb(m_configDir), QString());
    }
};

QTEST_MAIN(TestGetLocalAdb)
#include "tst_getadbpath.moc"
