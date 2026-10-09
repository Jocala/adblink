#ifndef WIRELESSPAIRING_H
#define WIRELESSPAIRING_H

#include <QString>
#include <QStringList>

// Android 11+ "Wireless debugging" pairing (adb pair HOST:PORT CODE).
//
// On the device: Settings > Developer options > Wireless debugging >
// "Pair device with pairing code" shows an IP:port and a 6-digit code.
// The main Wireless debugging screen shows a different IP:port to connect to.

namespace WirelessPairing {

struct Request {
    QString host;         // device IP or host name
    QString pairPort;     // port shown in the pairing popup
    QString code;         // 6-digit pairing code
    QString connectPort;  // optional: port on the main Wireless debugging screen
};

struct Result {
    bool paired = false;
    bool connected = false;   // only attempted when connectPort is set
    QString pairOutput;
    QString connectOutput;
};

// Empty string if the request is valid, otherwise a message for the user.
QString validate(const Request &req);

// Argument lists for QProcess::start(adbPath, args) - no shell involved.
QStringList pairArguments(const Request &req);
QStringList connectArguments(const Request &req);

bool pairSucceeded(const QString &adbOutput);
bool connectSucceeded(const QString &adbOutput);

// Run adb pair (and adb connect if connectPort is set). Blocks with the
// event loop running; each step is killed after timeoutMs.
Result run(const QString &adbPath, const Request &req, int timeoutMs = 30000);

// One-line, user-facing summary of an adb failure (strips "adb: " noise).
QString describeFailure(const QString &adbOutput);

} // namespace WirelessPairing

#endif // WIRELESSPAIRING_H
