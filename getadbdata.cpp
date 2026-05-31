#include "getadbdata.h"
#include "adbutils.h"
#include "logfile.h"
#include <QProcess>
#include <QEventLoop>

QString getadbOutput(const QString &cstring)
{
  QStringList args = QProcess::splitCommand(cstring);
  QString program = args.takeFirst();

  QProcess run_command;
  run_command.setProcessChannelMode(QProcess::MergedChannels);
  run_command.start(program, args);
  run_command.waitForStarted();

  QEventLoop loop;
  QObject::connect(&run_command, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
  QObject::connect(&run_command, &QProcess::errorOccurred, &loop, &QEventLoop::quit);
  loop.exec();

  QString command = run_command.readAll();
  return command;
}
