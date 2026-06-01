#include "getadbdata.h"
#include "adbutils.h"
#include "logfile.h"
#include <QProcess>

QString getadbOutput(const QString &cstring)
{
  QStringList args = QProcess::splitCommand(cstring);
  QString program = args.takeFirst();

  QProcess run_command;
  run_command.setProcessChannelMode(QProcess::MergedChannels);
  run_command.start(program, args);
  run_command.waitForStarted();

  syncWaitForProcess(run_command);

  QString command = run_command.readAll();
  return command;
}
