#include "Floor.h"

#include <QObject>
#include <QVector>
#include <QWidget>

#include "Building.h"
#include "DataButton.h"

// TODO: cleanup
Floor::Floor(int i, int fn, Building *parentBuilding, QObject *parent)
    : QObject(parent),
      index(i),
      floorNumber(fn),
      upButton(new DataButton(true, true, false, "UP ▲")),
      downButton(new DataButton(true, true, false, "DOWN ▼")),
      parentBuilding(parentBuilding) {
    // Disable buttons appropriately at the very top or bottom floor.
    if (index == 0)
        upButton->setDisabled(true);  // Top floor
    else if (index == (parentBuilding->floorCount - 1))
        downButton->setDisabled(true);  // Bottom floor

    connect(upButton, &DataButton::buttonCheckedUpdate, this,
            &Floor::floorStateChanged);
    connect(downButton, &DataButton::buttonCheckedUpdate, this,
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
