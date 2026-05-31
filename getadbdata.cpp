#include "getadbdata.h"
#include "adbutils.h"
#include "logfile.h"
#include <QDebug>
#include <QProcess>
#include <QCoreApplication>

struct adbReturn {
    int returncode;
    QString returnText;
};


QString getadbOutput(const QString &cstring)
{

  QStringList args = QProcess::splitCommand(cstring);
  QString program = args.takeFirst();

  QProcess run_command;

  run_command.setProcessChannelMode(QProcess::MergedChannels);

  run_command.start(program, args);

  run_command.waitForStarted();

  while(run_command.state() != QProcess::NotRunning)
      qApp->processEvents();

  QString command=run_command.readAll();

  return command;

}
