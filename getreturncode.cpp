#include "getreturncode.h"

#include "logfile.h"
#include <QDebug>
#include <QProcess>
#include <QCoreApplication>

bool getreturncode(const QString &cstring)
{
  QStringList args = QProcess::splitCommand(cstring);
  QString program = args.takeFirst();

  QProcess run_command;
  run_command.setProcessChannelMode(QProcess::MergedChannels);
  run_command.start(program, args);

  run_command.waitForStarted();

  while(!run_command.waitForFinished(50))
      qApp->processEvents();

  return (run_command.exitCode() == 0);
}
