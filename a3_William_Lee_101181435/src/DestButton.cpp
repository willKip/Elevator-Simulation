#include "DestButton.h"

#include <QObject>
#include <QString>

DestButton::DestButton(int destFloorNum, QString objectName, QWidget *parent)
    : DataButton(true, false, false, objectName, parent),
      destFloorNum(destFloorNum) {
    // Set destination floor number as button label
    setText(QString("%1").arg(destFloorNum));
}