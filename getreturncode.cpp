#include "getreturncode.h"

#include "logfile.h"
#include <QProcess>
#include "adbutils.h"

bool getreturncode(const QString &cstring)
{
  QStringList args = QProcess::splitCommand(cstring);
  QString program = args.takeFirst();

  QProcess run_command;
  run_command.setProcessChannelMode(QProcess::MergedChannels);
  run_command.start(program, args);
  run_command.waitForStarted();

  syncWaitForProcess(run_command);

  return (run_command.exitCode() == 0);
}
