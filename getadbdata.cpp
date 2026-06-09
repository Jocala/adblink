#include "getadbdata.h"
#include "adbutils.h"
#include <QProcess>

QString getadbOutput(const QString &cstring)
{
  QProcess run_command;
  run_command.setProcessChannelMode(QProcess::MergedChannels);
  run_command.startCommand(cstring);
  run_command.waitForStarted();

  syncWaitForProcess(run_command);

  QString command = run_command.readAll();
  return command;
}
