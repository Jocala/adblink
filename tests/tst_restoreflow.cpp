// End-to-end test of Restore + safety snapshot against a fake adb.
//
// The fake adb maps the device's /sdcard onto a temporary folder, runs
// "shell" commands with /bin/sh, and implements pull/push with plain copies,
// printing the same summary lines as real adb. Dialogs are answered
// automatically. Linux/macOS only (needs /bin/sh and python3).

#include <QtTest>
#include <QApplication>
#include <QMessageBox>
#include <QAbstractButton>
#include <QTemporaryDir>
#include <QTimer>

#include "backupmanager.h"
#include "devicerecord.h"
#include "safetysnapshot.h"

static const char *kFakeAdb = R"PY(#!/usr/bin/env python3
import os, shutil, subprocess, sys
root = os.environ["FAKE_DEVICE_ROOT"]
args = sys.argv[1:]
if args[:1] == ["-s"]:
    args = args[2:]
def dev(p):
    return p.replace("/sdcard", root + "/sdcard", 1) if p.startswith("/sdcard") else p
def count(d):
    return sum(len(f) for _, _, f in os.walk(d))
cmd = args[0] if args else ""
if cmd == "shell":
    line = " ".join(args[1:])
    if "busybox" in line or line.startswith("getprop"):
        sys.exit(0)
    if line.startswith("pm list packages"):
        print("package:org.xbmc.kodi"); sys.exit(0)
    line = line.replace("/sdcard", root + "/sdcard")
    r = subprocess.run(["/bin/sh", "-c", line], capture_output=True, text=True)
    sys.stdout.write((r.stdout + r.stderr).replace(root, ""))
    sys.exit(r.returncode)
if cmd == "pull":
    src, dst = args[1], args[2]
    if os.environ.get("FAKE_PULL_FAIL"):
        print("adb: error: failed to read copy response"); sys.exit(1)
    s = dev(src)
    if s.endswith("/."): s = s[:-2]
    if not os.path.exists(s):
        print("adb: error: failed to stat remote object '%s': No such file or directory" % src); sys.exit(1)
    shutil.copytree(s, dst, dirs_exist_ok=True)
    print("%s: %d files pulled, 0 skipped. 9.9 MB/s (1234 bytes in 0.001s)" % (src, count(s))); sys.exit(0)
if cmd == "push":
    src, dst = args[1], args[2]
    if os.environ.get("FAKE_PUSH_FAIL"):
        print("adb: error: failed to copy: connection reset"); sys.exit(1)
    s = src[:-2] if src.endswith("/.") else src
    d = dev(dst)
    if os.path.isdir(s):
        shutil.copytree(s, d, dirs_exist_ok=True)
    else:
        os.makedirs(os.path.dirname(d.rstrip("/")) or "/", exist_ok=True); shutil.copy(s, d)
    print("%s: %d files pushed, 0 skipped. 9.9 MB/s (1234 bytes in 0.001s)" % (src, count(s) if os.path.isdir(s) else 1)); sys.exit(0)
sys.exit(0)
)PY";

class TestRestoreFlow : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_tmp;
    QString m_deviceRoot;   // fake device filesystem
    QString m_configDir;    // ~/.jocala equivalent
    QString m_jsonPath;
    QString m_backupDir;    // the backup being restored
    QStringList m_messages; // texts of message boxes shown
    bool m_answerNoToWarnings = false;
    QTimer m_dialogTimer;

    QString kodiDir() const
    {
        return m_deviceRoot + "/sdcard/Android/data/org.xbmc.kodi/files/.kodi";
    }

    static void writeFile(const QString &path, const QByteArray &data)
    {
        QDir().mkpath(QFileInfo(path).absolutePath());
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write(data);
    }

    static QByteArray readFile(const QString &path)
    {
        QFile f(path);
        return f.open(QIODevice::ReadOnly) ? f.readAll() : QByteArray();
    }

    void answerDialogs()
    {
        QWidget *w = QApplication::activeModalWidget();
        if (!w)
            return;
        if (auto *box = qobject_cast<QMessageBox *>(w)) {
            m_messages << box->text();
            QAbstractButton *b = nullptr;
            if (box->icon() == QMessageBox::Warning && m_answerNoToWarnings)
                b = box->button(QMessageBox::No);
            if (!b) b = box->button(QMessageBox::Yes);
            if (!b) b = box->button(QMessageBox::Ok);
            if (b) b->click();
        } else if (auto *dlg = qobject_cast<QDialog *>(w)) {
            dlg->accept();
        }
    }

    bool runRestore(const QString &presetDir)
    {
        BackupManager mgr;
        DeviceRecord device;
        device.daddr = "fake-device";
        device.xbmcpackage = "org.xbmc.kodi";
        const QString adbPrefix = "\"" + QCoreApplication::applicationDirPath()
                                  + "/adbfiles/adb\" -s fake-device ";
        return mgr.restoreDevice(nullptr, device, adbPrefix, false, m_jsonPath, nullptr,
                                 [](const QString &cmd, const QString &) {
                                     QProcess p;
                                     p.setProcessChannelMode(QProcess::MergedChannels);
                                     p.startCommand(cmd);
                                     p.waitForFinished(60000);
                                     return QString::fromUtf8(p.readAll());
                                 },
                                 presetDir);
    }

    QStringList snapshots() const
    {
        return QDir(BackupManager::safetySnapshotRoot(m_jsonPath, "fake-device"))
            .entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    }

private slots:
    void initTestCase()
    {
        QVERIFY(m_tmp.isValid());
        qputenv("HOME", m_tmp.path().toUtf8()); // keep getlocaladb() away from the real config
        m_deviceRoot = m_tmp.path() + "/device";
        m_configDir = m_tmp.path() + "/.jocala";
        m_jsonPath = m_configDir + "/adblink.json";
        QDir().mkpath(m_configDir);
        writeFile(m_jsonPath, "{}");
        qputenv("FAKE_DEVICE_ROOT", m_deviceRoot.toUtf8());

        const QString adb = QCoreApplication::applicationDirPath() + "/adbfiles/adb";
        writeFile(adb, kFakeAdb);
        QFile::setPermissions(adb, QFile::permissions(adb) | QFileDevice::ExeOwner);

        connect(&m_dialogTimer, &QTimer::timeout, this, &TestRestoreFlow::answerDialogs);
        m_dialogTimer.start(20);
    }

    void init()
    {
        QDir(m_deviceRoot).removeRecursively();
        QDir(m_configDir + "/safety").removeRecursively();
        QDir(m_tmp.path() + "/backup").removeRecursively();
        m_messages.clear();
        m_answerNoToWarnings = false;
        qunsetenv("FAKE_PULL_FAIL");
        qunsetenv("FAKE_PUSH_FAIL");

        // Device currently has the user's live Kodi setup
        writeFile(kodiDir() + "/userdata/sources.xml", "CURRENT");
        writeFile(kodiDir() + "/addons/a/addon.xml", "current addon");
        // Backup to restore from
        m_backupDir = m_tmp.path() + "/backup";
        writeFile(m_backupDir + "/userdata/sources.xml", "FROM-BACKUP");
        writeFile(m_backupDir + "/addons/b/addon.xml", "backup addon");
    }

    void restoreTakesSnapshotFirst()
    {
        QVERIFY(runRestore(m_backupDir));

        QCOMPARE(readFile(kodiDir() + "/userdata/sources.xml"), QByteArray("FROM-BACKUP"));

        const QStringList snaps = snapshots();
        QCOMPARE(snaps.size(), 1);
        const QString snap = BackupManager::safetySnapshotRoot(m_jsonPath, "fake-device") + "/" + snaps.first();
        QCOMPARE(readFile(snap + "/userdata/sources.xml"), QByteArray("CURRENT"));
        QCOMPARE(readFile(snap + "/addons/a/addon.xml"), QByteArray("current addon"));
        QVERIFY(m_messages.last().contains("Undo last restore"));
    }

    void undoPutsPreviousDataBack()
    {
        QVERIFY(runRestore(m_backupDir));
        QCOMPARE(readFile(kodiDir() + "/userdata/sources.xml"), QByteArray("FROM-BACKUP"));

        const QString root = BackupManager::safetySnapshotRoot(m_jsonPath, "fake-device");
        const QString latest = SafetySnapshot::latestSnapshot(snapshots());
        QTest::qWait(1100); // next snapshot gets a new timestamp
        QVERIFY(runRestore(root + "/" + latest));

        QCOMPARE(readFile(kodiDir() + "/userdata/sources.xml"), QByteArray("CURRENT"));
        QVERIFY(!QFile::exists(kodiDir() + "/addons/b/addon.xml")); // backup's addon gone
        QCOMPARE(snapshots().size(), 2); // the undo was itself snapshotted
    }

    void failedPushLeavesRecoverableSnapshot()
    {
        qputenv("FAKE_PUSH_FAIL", "1");
        QVERIFY(!runRestore(m_backupDir));

        // Device folder was wiped, but the snapshot holds the old data
        QVERIFY(!QFile::exists(kodiDir() + "/userdata/sources.xml"));
        QCOMPARE(snapshots().size(), 1);
        const QString snap = BackupManager::safetySnapshotRoot(m_jsonPath, "fake-device") + "/" + snapshots().first();
        QCOMPARE(readFile(snap + "/userdata/sources.xml"), QByteArray("CURRENT"));
        bool told = false;
        for (const QString &m : m_messages)
            told = told || m.contains("Undo last restore");
        QVERIFY(told);
    }

    void failedSnapshotAbortsWithoutDeleting()
    {
        qputenv("FAKE_PULL_FAIL", "1");
        m_answerNoToWarnings = true;
        QVERIFY(!runRestore(m_backupDir));

        // Nothing was deleted
        QCOMPARE(readFile(kodiDir() + "/userdata/sources.xml"), QByteArray("CURRENT"));
        QVERIFY(snapshots().isEmpty());
    }

    void prunesToKeepSetting()
    {
        writeFile(m_jsonPath, "{\"safetysnapshots\":\"2\"}");
        // runRestore passes no KodiDataManager, so the setting can't be read;
        // pre-seed old snapshots and check the default (3) is applied.
        const QString root = BackupManager::safetySnapshotRoot(m_jsonPath, "fake-device");
        for (const char *old : { "2020-01-01_000000", "2020-01-02_000000", "2020-01-03_000000" })
            writeFile(root + "/" + old + "/userdata/x", "old");
        QVERIFY(runRestore(m_backupDir));
        const QStringList snaps = snapshots();
        QCOMPARE(snaps.size(), SafetySnapshot::DefaultKeep);
        QVERIFY(!snaps.contains("2020-01-01_000000"));
        writeFile(m_jsonPath, "{}");
    }

    void freshDeviceNeedsNoSnapshot()
    {
        QDir(m_deviceRoot).removeRecursively();
        QDir().mkpath(m_deviceRoot + "/sdcard/Android/data/org.xbmc.kodi");
        QVERIFY(runRestore(m_backupDir));
        QCOMPARE(readFile(kodiDir() + "/userdata/sources.xml"), QByteArray("FROM-BACKUP"));
        QVERIFY(snapshots().isEmpty());
    }
};

QTEST_MAIN(TestRestoreFlow)
#include "tst_restoreflow.moc"
