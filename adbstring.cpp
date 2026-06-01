#include "adbstring.h"

QString adbString(const QString& input) {
    QString result = input;
   result.replace("'", "\\'"); // Escape single quotes
    result = "\"" + result + "\""; // Wrap in double quotes
    return result;
}
