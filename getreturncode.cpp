#include "getreturncode.h"

#include "logfile.h"
#include <QProcess>
#include "adbutils.h"

bool getreturncode(const QString &cstring)
{
  QStringList args = QProcess::splitCommand(cstring);
  if (args.isEmpty())
    return false;
  QString program = args.takeFirst();

  QProcess run_command;
  run_command.setProcessChannelMode(QProcess::MergedChannels);
  run_command.start(program, args);
  if (!run_command.waitForStarted())
    return false;

  syncWaitForProcess(run_command);

  if (run_command.error() == QProcess::FailedToStart)
    return false;

  return (run_command.exitCode() == 0);
}
