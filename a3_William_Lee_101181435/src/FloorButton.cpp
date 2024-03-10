#include "FloorButton.h"

#include <QObject>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>

#include "Building.h"
#include "Direction.h"

// Static definitions
const QString FloorButton::checkedStyleSheetStr =
    "background-color: rgba(10, 0, 135, 60%);"
    "color: rgb(255,255,255);";
const QString FloorButton::unCheckedStyleSheetStr = "";

FloorButton::FloorButton(int f, Direction d, bool c, QString objectName,
                         QWidget *parent)
    : QPushButton(parent), floorNum(f), direction(d), checked(c) {
    if (!objectName.isEmpty()) setObjectName(objectName);

    setText(directionToLabelText());

    // Set visual style for initial button state
    updateStyleSheet();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

bool FloorButton::isChecked() const { return checked; }

void FloorButton::setChecked(bool newState) {
    if (checked != newState) {
        checked = newState;
        updateStyleSheet();
    }
}

Direction FloorButton::getDirection() { return direction; }

int FloorButton::getFloorNum() { return floorNum; }

QString FloorButton::directionToLabelText() const {
    switch (direction) {
        case Direction::UP:
            return "UP ▲";
            break;
        case Direction::DOWN:
            return "DOWN ▼";
            break;
        default:
            throw "Error: Invalid direction given for floor button label text!";
    }
}

void FloorButton::updateStyleSheet() {
    if (checked)
        setStyleSheet(checkedStyleSheetStr);
    else
        setStyleSheet(unCheckedStyleSheetStr);
}
