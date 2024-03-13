#include "DestButton.h"

#include <QObject>
#include <QString>

DestButton::DestButton(int destFloorNum, QString objectName, QWidget *parent)
    : DataButton(true, false, false, objectName, parent),
      destFloorNum(destFloorNum) {
    setText(QString("%1").arg(destFloorNum));
}