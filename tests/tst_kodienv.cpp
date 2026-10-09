#include <QtTest>
#include "kodienv.h"

class TestKodiEnv : public QObject
{
    Q_OBJECT

private slots:
    void scopedPathOnSdcard()
    {
        // Issue #3: Fire TV Cube with Kodi moved to kodi_data
        QCOMPARE(parseXbmcEnvDataRoot("xbmc.data=/sdcard/kodi_data/org.xbmc.kodi/files\n",
                                      "org.xbmc.kodi"),
                 QStringLiteral("/sdcard/kodi_data/org.xbmc.kodi"));
    }

    void legacyPathOnSdcard()
    {
        QCOMPARE(parseXbmcEnvDataRoot("xbmc.data=/sdcard/Android/data/org.xbmc.kodi/files",
                                      "org.xbmc.kodi"),
                 QStringLiteral("/sdcard/Android/data/org.xbmc.kodi"));
    }

    void legacyPathOnExternalStorage()
    {
        QCOMPARE(parseXbmcEnvDataRoot("xbmc.data=/storage/1234-ABCD/Android/data/org.xbmc.kodi/files",
                                      "org.xbmc.kodi"),
                 QStringLiteral("/storage/1234-ABCD/Android/data/org.xbmc.kodi"));
    }

    void scopedPathOnExternalStorage()
    {
        // What Data Move writes when moving scoped data to an SD card
        QCOMPARE(parseXbmcEnvDataRoot("xbmc.data=/storage/1234-ABCD/kodi_data/org.xbmc.kodi/files",
                                      "org.xbmc.kodi"),
                 QStringLiteral("/storage/1234-ABCD/kodi_data/org.xbmc.kodi"));
    }

    void pathWithSpaceRejected()
    {
        // Would be split by the device shell, e.g. rm -r /storage/emulated/0/My ...
        QVERIFY(parseXbmcEnvDataRoot("xbmc.data=/storage/emulated/0/My Kodi/Android/data/org.xbmc.kodi/files",
                                     "org.xbmc.kodi").isEmpty());
    }

    void shellMetacharactersRejected()
    {
        QVERIFY(parseXbmcEnvDataRoot("xbmc.data=/sdcard/x;rm -rf /sdcard/*;/Android/data/org.xbmc.kodi/files",
                                     "org.xbmc.kodi").isEmpty());
        QVERIFY(parseXbmcEnvDataRoot("xbmc.data=/sdcard/$(reboot)/kodi_data/org.xbmc.kodi/files",
                                     "org.xbmc.kodi").isEmpty());
    }

    void parentDirectoryRejected()
    {
        QVERIFY(parseXbmcEnvDataRoot("xbmc.data=/sdcard/../data/kodi_data/org.xbmc.kodi/files",
                                     "org.xbmc.kodi").isEmpty());
    }

    void windowsLineEndingsAndTrailingSlash()
    {
        QCOMPARE(parseXbmcEnvDataRoot("xbmc.data=/sdcard/kodi_data/org.xbmc.kodi/files/\r\n",
                                      "org.xbmc.kodi"),
                 QStringLiteral("/sdcard/kodi_data/org.xbmc.kodi"));
    }

    void otherKeysAfterDataLine()
    {
        // Previously all lines were concatenated, corrupting the path
        QCOMPARE(parseXbmcEnvDataRoot("xbmc.data=/sdcard/kodi_data/org.xbmc.kodi/files\nxbmc.temp=/sdcard/tmp\n",
                                      "org.xbmc.kodi"),
                 QStringLiteral("/sdcard/kodi_data/org.xbmc.kodi"));
    }

    void otherKeysBeforeDataLine()
    {
        QCOMPARE(parseXbmcEnvDataRoot("xbmc.temp=/sdcard/tmp\nxbmc.data=/sdcard/kodi_data/org.xbmc.kodi/files\n",
                                      "org.xbmc.kodi"),
                 QStringLiteral("/sdcard/kodi_data/org.xbmc.kodi"));
    }

    void forkPackageName()
    {
        QCOMPARE(parseXbmcEnvDataRoot("xbmc.data=/sdcard/kodi_data/net.kodi.fork/files",
                                      "net.kodi.fork"),
                 QStringLiteral("/sdcard/kodi_data/net.kodi.fork"));
    }

    void wrongPackageRejected()
    {
        QVERIFY(parseXbmcEnvDataRoot("xbmc.data=/sdcard/kodi_data/org.xbmc.kodi/files",
                                     "net.kodi.fork").isEmpty());
    }

    void relativePathRejected()
    {
        QVERIFY(parseXbmcEnvDataRoot("xbmc.data=sdcard/kodi_data/org.xbmc.kodi/files",
                                     "org.xbmc.kodi").isEmpty());
    }

    void missingFilesSuffixRejected()
    {
        QVERIFY(parseXbmcEnvDataRoot("xbmc.data=/sdcard/kodi_data/org.xbmc.kodi",
                                     "org.xbmc.kodi").isEmpty());
    }

    void unknownStructureRejected()
    {
        QVERIFY(parseXbmcEnvDataRoot("xbmc.data=/sdcard/random/org.xbmc.kodi/files",
                                     "org.xbmc.kodi").isEmpty());
    }

    void noDataKey()
    {
        QVERIFY(parseXbmcEnvDataRoot("xbmc.temp=/sdcard/tmp\n", "org.xbmc.kodi").isEmpty());
    }

    void emptyContent()
    {
        QVERIFY(parseXbmcEnvDataRoot(QString(), "org.xbmc.kodi").isEmpty());
    }

    void adbErrorOutput()
    {
        QVERIFY(parseXbmcEnvDataRoot("cat: /sdcard/xbmc_env.properties: No such file or directory",
                                     "org.xbmc.kodi").isEmpty());
    }

    void emptyPackage()
    {
        QVERIFY(parseXbmcEnvDataRoot("xbmc.data=/sdcard/kodi_data/org.xbmc.kodi/files",
                                     QString()).isEmpty());
    }
};

QTEST_APPLESS_MAIN(TestKodiEnv)
#include "tst_kodienv.moc"
