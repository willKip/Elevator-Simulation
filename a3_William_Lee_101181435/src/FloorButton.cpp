#include "FloorButton.h"

#include <QObject>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>

#include "Building.h"
#include "Direction.h"

FloorButton::FloorButton(int f, Direction d, bool c, QString objectName,
                         QWidget *parent)
    : DataButton(true, true, c, objectName, parent), floorNum(f), direction(d) {
    setText(directionToLabelText());
}

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

void FloorButton::updateStyleSheet() { DataButton::updateStyleSheet(); }
