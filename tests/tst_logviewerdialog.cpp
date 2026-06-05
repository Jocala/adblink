#include <QtTest>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QClipboard>
#include <QApplication>
#include <QTimer>
#include <QTextBrowser>
#include "logviewerdialog.h"

class TestLogViewerDialog : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_tmp;
    LogViewerDialog *dialog = nullptr;
    QString m_jocalaDir;
    QString m_firstLogPath;
    QString m_secondLogPath;

    void writeFile(const QString &path, const QString &content)
    {
        QFile file(path);
        QVERIFY2(file.open(QIODevice::WriteOnly | QIODevice::Text),
                 qPrintable("Cannot write " + path));
        QTextStream out(&file);
        out << content;
        file.close();
    }

    QString readBrowser() const
    {
        return dialog->findChild<QTextBrowser *>()->toPlainText();
    }

private slots:
    void initTestCase()
    {
        QVERIFY(m_tmp.isValid());
#ifdef Q_OS_WIN
        m_jocalaDir = m_tmp.path() + QStringLiteral("/AppData/Roaming/.jocala");
#else
        m_jocalaDir = m_tmp.path() + QStringLiteral("/.jocala");
#endif
        QDir().mkpath(m_jocalaDir);
    }

    void init()
    {
        dialog = new LogViewerDialog(nullptr);
        dialog->setLogDirForTest(m_jocalaDir + QStringLiteral("/"));
    }

    void cleanup()
    {
        delete dialog;
        dialog = nullptr;
        QApplication::clipboard()->clear();
    }

    void defaultState()
    {
        QCOMPARE(dialog->windowTitle(), QStringLiteral("Log viewer"));
    }

    void loadAdblinkLogExists()
    {
        const QString content = QStringLiteral("line1\nline2\nline3");
        writeFile(m_jocalaDir + QStringLiteral("/adblink.log"), content);

        dialog->loadAdblinkLog();
        QCOMPARE(readBrowser(), content);
    }

    void loadAdblinkLogMissing()
    {
        // No log file in the temp dir; loadAdblinkLog shows an error dialog
        // then returns. We schedule a timer to dismiss the dialog.
        QTimer::singleShot(200, this, [this] {
            QApplication::activeModalWidget()->close();
        });
        dialog->loadAdblinkLog();
        // Should not crash — browser stays empty or shows previous content
    }

    void copyClickedSetsClipboard()
    {
        const QString content = QStringLiteral("copy test content");
        writeFile(m_jocalaDir + QStringLiteral("/adblink.log"), content);

        dialog->loadAdblinkLog();
        QMetaObject::invokeMethod(dialog, "copyClicked");
        QCOMPARE(QApplication::clipboard()->text(), content);
    }

    void swapClickedTogglesBetweenLogs()
    {
        writeFile(m_jocalaDir + QStringLiteral("/adblink.log"),
                  QStringLiteral("first log content"));
        writeFile(m_jocalaDir + QStringLiteral("/adblink.old.log"),
                  QStringLiteral("second log content"));

        dialog->loadAdblinkLog();
        QCOMPARE(readBrowser(), QStringLiteral("first log content"));

        QMetaObject::invokeMethod(dialog, "swapClicked");
        QCOMPARE(readBrowser(), QStringLiteral("second log content"));

        QMetaObject::invokeMethod(dialog, "swapClicked");
        QCOMPARE(readBrowser(), QStringLiteral("first log content"));
    }
};

QTEST_MAIN(TestLogViewerDialog)
#include "tst_logviewerdialog.moc"
