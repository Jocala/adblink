#include "getreturncode.h"

#include "logfile.h"
#include <QProcess>
#include <QEventLoop>

bool getreturncode(const QString &cstring)
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

  return (run_command.exitCode() == 0);
}
