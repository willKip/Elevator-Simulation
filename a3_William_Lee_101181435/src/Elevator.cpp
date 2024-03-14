#include "Elevator.h"

#include <QMap>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>
#include <QWidget>
#include <algorithm>

#include "Building.h"
#include "DataButton.h"
#include "DestButton.h"

Elevator::Elevator(int buildingColIndex, int carId, int initialFloorNum,
                   Building *parentBuilding, QObject *parent)
    : QObject(parent),
      buildingColIndex(buildingColIndex),
      carId(carId),
      currentFloorNum(initialFloorNum),
      parentBuilding(parentBuilding),
      openButton(new DataButton(false, true, false, "")),
      closeButton(new DataButton(false, true, false, "")),
      currentMovement(MovementState::STOPPED),
      doorState(DoorState::CLOSED),
      emergencyState(EmergencyState::NONE),
      movementTimer(new QTimer(this)),
      doorSpeedTimer(new QTimer(this)),
      doorWaitTimer(new QTimer(this)) {
    // Set up buttons
    openButton->setText("Open ❰|❱");
    closeButton->setText("Close ❱|❰");

    for (int f_ind = 0; f_ind < parentBuilding->floorCount; ++f_ind) {
        int floorNum = parentBuilding->index_to_floorNum(f_ind);
        DestButton *destButton = new DestButton(floorNum);

        destinationButtons.insert(floorNum, destButton);

        connect(destButton, &DestButton::buttonCheckedChanged, this,
                &Elevator::determineMovement);
    }

    // Set up timers
    movementTimer->setInterval(movementMs);
    doorSpeedTimer->setInterval(doorSpeedMs);
    doorWaitTimer->setInterval(doorWaitMs);

    // Complete the transition of door state if speedTimer completes without
    // interruption.
    connect(doorSpeedTimer, &QTimer::timeout, this, [this]() {
        switch (this->doorState) {
            case DoorState::CLOSING:
                this->setDoorState(DoorState::CLOSED);
                break;
            case DoorState::OPENING:
                this->setDoorState(DoorState::OPEN);
                this->doorWaitTimer->start();
                break;
            default:
                break;
        }
    });

    // Connect door override buttons
    connect(openButton, &DataButton::buttonCheckedChanged, this,
            &Elevator::openDoors);
    connect(closeButton, &DataButton::buttonCheckedChanged, this,
            &Elevator::closeDoors);

    // If the movement timer completes without interruption, finalize the
    // movement and reflect the new position in the building.
    connect(movementTimer, &QTimer::timeout, this, [this]() {
        switch (currentMovement) {
            case MovementState::UPWARDS:
                ++currentFloorNum;
                emit elevatorDataChanged();
                break;
            case MovementState::DOWNWARDS:
                --currentFloorNum;
                emit elevatorDataChanged();
                break;
            default:
                break;
        }
    });

    // If door has been open until timer expiry, start closing door.
    connect(doorWaitTimer, &QTimer::timeout, this, [this]() {
        if (this->doorState == DoorState::OPEN) this->closeDoors();
    });
}

const QString Elevator::getElevatorString() const {
    QString movementStr;
    QString doorStr;
    QString emergencyStr;

    switch (currentMovement) {
        case Elevator::MovementState::STOPPED:
            movementStr = "STOP -";
            break;
        case Elevator::MovementState::UPWARDS:
            movementStr = "UP ▲";
            break;
        case Elevator::MovementState::DOWNWARDS:
            movementStr = "DOWN ▼";
            break;
        default:
            throw "ERROR: Invalid Movement enum";
    }
    switch (doorState) {
        case Elevator::DoorState::CLOSED:
            doorStr = "(closed)";
            break;
        case Elevator::DoorState::CLOSING:
            doorStr = "(closing...)";
            break;
        case Elevator::DoorState::OPENING:
            doorStr = "(opening...)";
            break;
        case Elevator::DoorState::OPEN:
            doorStr = "(open)";
            break;
        default:
            throw "ERROR: Invalid door state enum";
    }
    switch (emergencyState) {
        case Elevator::EmergencyState::NONE:
            emergencyStr = "";
            break;
        case Elevator::EmergencyState::FIRE:
            emergencyStr = "\nFIRE";
            break;
        case Elevator::EmergencyState::POWER_OUT:
            emergencyStr = "\nPOWER OUT";
            break;
        case Elevator::EmergencyState::OVERLOAD:
            emergencyStr = "\nOVERLOAD";
            break;
        case Elevator::EmergencyState::DOOR_STUCK:
            emergencyStr = "\nDOOR OBSTACLE";
            break;
        case Elevator::EmergencyState::HELP:
            emergencyStr = "\nHELP";
            break;
        default:
            throw "ERROR: Invalid emergency enum";
    }
    return QString("%1\n%2%3").arg(movementStr, doorStr, emergencyStr);
}

bool Elevator::isMoving() const {
    return currentMovement != MovementState::STOPPED;
}

void Elevator::determineMovement() {
    // Collect floors with their up/down floor buttons pressed or targeted by
    // this elevator's destination button panel.
    QVector<int> queuedFloors = parentBuilding->getQueuedFloors();
    queuedFloors.append(queuedDestinations());

    // No eligible floors queued
    if (queuedFloors.isEmpty()) {
        setMovement(MovementState::STOPPED);
        return;
    }

    // Sort floors
    std::sort(queuedFloors.begin(), queuedFloors.end());

    // Remove duplicate floors
    queuedFloors.erase(std::unique(queuedFloors.begin(), queuedFloors.end()),
                       queuedFloors.end());

    // Compute optimal floor to move to
    int targetFloor = closestQueuedFloor(queuedFloors);

    if (currentFloorNum == targetFloor) {
        // Elevator stopped on current floor.
        setMovement(MovementState::STOPPED);
        openDoors();
        destinationButtons[currentFloorNum]->setChecked(false);
        emit elevatorArrived();
    } else if (doorState == DoorState::CLOSED) {
        // Elevator ready to move to other floor.
        if (currentFloorNum < targetFloor) {
            setMovement(MovementState::UPWARDS);
        } else if (currentFloorNum > targetFloor) {
            setMovement(MovementState::DOWNWARDS);
        }
    }
}

void Elevator::ring() { emit textOut(QString("*ring!*")); }

void Elevator::openDoors() {
    // Only attempt to open doors if the elevator is not moving
    if (isMoving()) return;

    switch (doorState) {
        case DoorState::CLOSED:
        case DoorState::CLOSING:
            // Open the doors and ring bell
            setDoorState(DoorState::OPENING);
            doorSpeedTimer->start();  // Start door movement
            ring();
            break;
        case DoorState::OPEN:
            // Extend open time (reset timer)
            doorWaitTimer->start();
            break;
        case DoorState::OPENING:
            // Already opening, no effect.
            break;
        default:
            throw "ERROR: Elevator in impossible DoorState";
            break;
    }
}

void Elevator::closeDoors() {
    // Doors would already be closed if elevator is moving
    if (isMoving()) return;

    switch (doorState) {
        case DoorState::OPEN:
        case DoorState::OPENING:
            // Close the doors and ring bell
            // TODO: handle obstacle state
            setDoorState(DoorState::CLOSING);
            doorWaitTimer->stop();    // Door timeout not relevant anymore
            doorSpeedTimer->start();  // Start door movement
            ring();
            break;
        case DoorState::CLOSED:
        case DoorState::CLOSING:
            // Already closing or closed, no effect.
            break;
        default:
            throw "ERROR: Elevator in impossible DoorState";
            break;
    }
}

void Elevator::setMovement(Elevator::MovementState newMovement) {
    if (currentMovement != newMovement) {
        currentMovement = newMovement;

        if (isMoving())
            this->movementTimer->start();
        else
            this->movementTimer->stop();

        emit elevatorDataChanged();
    }
}

void Elevator::setDoorState(Elevator::DoorState newDoorState) {
    if (doorState != newDoorState) {
        doorState = newDoorState;
        emit elevatorDataChanged();
    }
}

const QVector<int> Elevator::queuedDestinations() const {
    QVector<int> queued;

    for (auto i = destinationButtons.cbegin(), end = destinationButtons.cend();
         i != end; ++i)
        if (i.value()->isChecked()) queued.append(i.key());

    return queued;
}

int Elevator::closestQueuedFloor(const QVector<int> &floors) const {
    if (floors.isEmpty())
        throw "ERROR: No floors are queued, cannot find closest queued";
    // TODO: ^, whether elevator was moving or idle determines whether it
    // opens door or not.

    // Current floor is below or above all queued floors.
    if (currentFloorNum <= floors.first()) return floors.first();
    if (currentFloorNum >= floors.last()) return floors.last();

    // Current floor is between two queued floors, before and after.
    const int *closestIt = std::adjacent_find(
        floors.begin(), floors.end(), [this](int before, int after) {
            return currentFloorNum >= before && currentFloorNum <= after;
        });

    int closestBefore = *closestIt;       // Closest queued before current
    int closestAfter = *(closestIt + 1);  // Closest queued after current

    // Distances to each floor.
    int distBefore = currentFloorNum - closestBefore;
    int distAfter = closestAfter - currentFloorNum;

    // Return the floor that is closer. In case of a tie, floor that was on
    // the direction the elevator was moving is prioritized. If the elevator
    // somehow had no direction it was moving in, lower floor is
    // prioritized.
    if (distBefore < distAfter) {
        return closestBefore;
    } else if (distAfter < distBefore) {
        return closestAfter;
    } else {
        // Distance tied
        if (currentMovement == MovementState::UPWARDS)
            return closestAfter;
        else
            return closestBefore;
    }
}

QVector<QWidget *> Elevator::getDoorButtonWidgets() {
    return QVector<QWidget *>{qobject_cast<QWidget *>(openButton),
                              qobject_cast<QWidget *>(closeButton)};
}

QVector<QWidget *> Elevator::getDestButtonWidgets() {
    QVector<QWidget *> toReturn;

    for (auto i = destinationButtons.cbegin(), end = destinationButtons.cend();
         i != end; ++i)
        toReturn.append(qobject_cast<QWidget *>(i.value()));

    return toReturn;
}

const QString Elevator::getTextDisplay() const {
    /* Emergency */
    switch (emergencyState) {
        case EmergencyState::HELP:
            return "HELP: (connecting to building safety service or 911...)";
        case EmergencyState::FIRE:
            if (currentMovement == MovementState::STOPPED)
                return "FIRE: Safe floor reached. Please disembark.";
            else
                return "FIRE: Moving to safe floor.";
        case EmergencyState::POWER_OUT:
            if (currentMovement == MovementState::STOPPED)
                return "POWER OUTAGE: Safe floor reached. Please disembark.";
            else
                return "POWER OUTAGE: Running on emergency power. Moving to "
                       "safe floor.";
        case EmergencyState::OVERLOAD:
            return "OVERLOAD: Please reduce the load.";
        case EmergencyState::DOOR_STUCK:  // TODO
        case EmergencyState::NONE:
        default:
            break;
    }

    switch (currentMovement) {
        case MovementState::UPWARDS:
            return "▲ Moving UP...";
        case MovementState::DOWNWARDS:
            return "▼ Moving DOWN...";
        case MovementState::STOPPED:
            return "- Stopped.";
        default:
            break;
    }

    return "";
}

void Elevator::setEmergency(EmergencyState state) {
    if (emergencyState != state) {
        emergencyState = state;
        emit elevatorDataChanged();

        // Audio warnings (using inline console instead of actual audio output)
        switch (emergencyState) {
            case EmergencyState::FIRE:
                if (currentMovement == MovementState::STOPPED)
                    emit textOut("Safe floor reached. Please disembark.");
                else
                    emit textOut(
                        "A fire has been detected. Moving towards safe floor.");
            case EmergencyState::POWER_OUT:
                if (currentMovement == MovementState::STOPPED)
                    emit textOut("Safe floor reached. Please disembark.");
                else
                    emit textOut(
                        "A power outage has been detected. Moving towards safe "
                        "floor.");
            case EmergencyState::OVERLOAD:
                emit textOut("Overload. Please reduce the load.");
            case EmergencyState::HELP:
                emit textOut("(connected to building safety service or 911)");
            case EmergencyState::DOOR_STUCK:  // TODO
            case EmergencyState::NONE:
            default:
                break;
        }
    }
}
