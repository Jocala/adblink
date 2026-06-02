#ifndef IPTABLEWIDGETITEM_H
#define IPTABLEWIDGETITEM_H

#include <QTableWidgetItem>

class IpTableWidgetItem : public QTableWidgetItem {
public:
    IpTableWidgetItem(const QString &text) : QTableWidgetItem(text) {}

    bool operator<(const QTableWidgetItem &other) const override {
        QString thisText = text();
        QString otherText = other.text();

        if (thisText == "N/A") return false;
        if (otherText == "N/A") return true;

        QStringList thisOctets = thisText.split(".");
        QStringList otherOctets = otherText.split(".");

        for (int i = 0; i < 4; ++i) {
            if (i >= thisOctets.size() || i >= otherOctets.size()) {
                return thisOctets.size() < otherOctets.size();
            }
            int thisVal = thisOctets[i].toInt();
            int otherVal = otherOctets[i].toInt();
            if (thisVal != otherVal) {
                return thisVal < otherVal;
            }
        }
        return false;
    }
};

#endif // IPTABLEWIDGETITEM_H
