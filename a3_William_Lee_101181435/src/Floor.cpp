#include "Floor.h"

#include <QVector>
#include <QWidget>

#include "Building.h"

Floor::Floor(int i, int fn, Building *parentBuilding, QObject *parent)
    : QObject(parent),
      index(i),
      floorNumber(fn),
      upButton(new FloorButton(floorNumber, Direction::UP, false,
                               QString("floor%1UpButton").arg(floorNumber))),
      downButton(
          new FloorButton(floorNumber, Direction::DOWN, false,
                          QString("floor%1DownButton").arg(floorNumber))),
      parentBuilding(parentBuilding) {
    // Disable buttons appropriately at the very top or bottom floor.
    if (index == 0)
        upButton->setDisabled(true);  // Top floor
    else if (index == (parentBuilding->floorCount - 1))
        downButton->setDisabled(true);  // Bottom floor

    connect(upButton, &FloorButton::buttonCheckedChanged, this,
            &Floor::floorStateChanged);
    connect(downButton, &FloorButton::buttonCheckedChanged, this,
            &Floor::floorStateChanged);
};

QVector<QWidget *> Floor::getButtonWidgets() {
    return QVector<QWidget *>{qobject_cast<QWidget *>(upButton),
                              qobject_cast<QWidget *>(downButton)};
}

bool Floor::pressedUp() const { return upButton->isChecked(); }

bool Floor::pressedDown() const { return downButton->isChecked(); }

void Floor::resetButtons() {
    upButton->setChecked(false);
    downButton->setChecked(false);
}
