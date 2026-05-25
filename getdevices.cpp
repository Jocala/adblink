#include "getdevices.h"
#include "getadbdata.h"
#include "logfile.h"
#include "adbutils.h"
#include "getlocaladb.h"
#include <QDebug>
#include <QRegularExpression>

getdevices::getdevices(QObject *parent)
    : QObject{parent}
{

    QString cstring;
    QString command;
    QStringList mstringlist;
    QStringList dstringlist;

    cstring = "null devices";
    command = getadbOutput(cstring);

    mstringlist = command.split(QRegularExpression("[\t\n\r]"), Qt::SkipEmptyParts);

    if (command.contains("List of devices attached"))
    {
        mstringlist.removeFirst();
        for (int a = 0; a < mstringlist.size(); a = a + 2)
        {
            QStringList pieces = mstringlist.at(a).split(":", Qt::SkipEmptyParts);
            if (!mstringlist.at(a).contains("daemon"))
                dstringlist << pieces.at(0);
        }
    }

}
