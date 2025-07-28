#include "Elevator.h"

#include <QBrush>
#include <QMap>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>
#include <QWidget>
#include <algorithm>

#include "Building.h"
#include "DataButton.h"

Elevator::Elevator(int initialFloorNum, Building *parentBuilding,
                   QObject *parent)
    : QObject(parent),
      parentBuilding(parentBuilding),
      openButton(new DataButton(false, true, false, "Open ❰|❱")),
      closeButton(new DataButton(false, true, false, "Close ❱|❰")),
      fireButton(new DataButton(true, false, false, "FIRE")),
      obstacleButton(new DataButton(true, false, false, "DOOR\n\nOBST\nACLE")),
      helpButton(new DataButton(true, false, false, "HELP")),
      overloadButton(new DataButton(true, false, false, "OVER\nLOAD")),
      currentMovement(MovementState::STOPPED),
      currentDoor(DoorState::CLOSED),
      currentEmergency(EmergencyState::NONE),
      movementTimer(new QTimer(this)),
      doorSpeedTimer(new QTimer(this)),
      doorWaitTimer(new QTimer(this)),
      doorCloseFailures(0),
      currentFloorNum(initialFloorNum) {
    // Set initial obstacle simulation button state.
    obstacleButton->setDisabled(currentDoor == DoorState::CLOSED ? true
                                                                 : false);

    // Connect door override buttons to their functionality slots
    connect(openButton, &DataButton::buttonCheckedUpdate, this,
            &Elevator::openDoors);
    connect(closeButton, &DataButton::buttonCheckedUpdate, this,
            &Elevator::closeDoors);

    // Connect emergency buttons to the update slot
    connect(fireButton, &DataButton::buttonCheckedUpdate, this,
            &Elevator::updateEmergency);
    connect(obstacleButton, &DataButton::buttonCheckedUpdate, this,
            &Elevator::updateEmergency);
    connect(helpButton, &DataButton::buttonCheckedUpdate, this,
            &Elevator::updateEmergency);
    connect(overloadButton, &DataButton::buttonCheckedUpdate, this,
            &Elevator::updateEmergency);

    // Compute new movement when building data has changed
    connect(parentBuilding, &Building::buildingDataChanged, this,
            &Elevator::determineMovement);

    // Initialize destination buttons and connect them to elevator updates.
    for (int f_ind = 0; f_ind < parentBuilding->floorCount; ++f_ind) {
        int floorNum = parentBuilding->index_to_floorNum(f_ind);

        DataButton *destButton =
            new DataButton(true, false, false, QString("%1").arg(floorNum));

        destinationButtons.insert(floorNum, destButton);

        connect(destButton, &DataButton::buttonCheckedUpdate, this,
                &Elevator::determineMovement);
    }

    /* Set up timers */
    movementTimer->setInterval(movementMs);
    doorSpeedTimer->setInterval(doorSpeedMs);
    doorWaitTimer->setInterval(doorWaitMs);

    // Behavior for elevator movement timer timeout (elevator movement complete)
    connect(movementTimer, &QTimer::timeout, this, [this]() {
        switch (currentMovement) {
            case MovementState::UPWARDS:
                // Elevator finishes upward movement.
                ++currentFloorNum;
                emit elevatorDataChanged();
                break;
            case MovementState::DOWNWARDS:
                // Elevator finishes downward movement.
                --currentFloorNum;
                emit elevatorDataChanged();
                break;
            default:
                break;
        }
    });

    // Behavior for door movement timer timeout (door transition complete)
    connect(doorSpeedTimer, &QTimer::timeout, this, [this]() {
        switch (this->currentDoor) {
            case DoorState::CLOSING:
                // Check sensors to see if door closure can be completed
                if (this->doorSensorSeesObstacle()) {
                    // Obstacle detected, abort and open again
                    this->doorCloseFailures++;
                    emit this->textOut(QString("(Light sensors detected "
                                               "obstacle! Failures: %1/%2)")
                                           .arg(this->doorCloseFailures)
                                           .arg(this->doorCloseFailThreshold));
                    this->openDoors();
                } else {
                    // Successfully closed
                    this->doorCloseFailures = 0;
                    this->setDoorState(DoorState::CLOSED);
                }

                // Door obstacle state may have been triggered or cleared
                this->updateEmergency();
                break;
            case DoorState::OPENING:
                // Successfully opened
                this->setDoorState(DoorState::OPEN);

                this->doorWaitTimer->start();  // Start idle timer
                break;
            default:
                break;
        }
    });

    // Behavior for door wait timer timeout (doors automatically closing)
    connect(doorWaitTimer, &QTimer::timeout, this, [this]() {
        if (this->currentDoor == DoorState::OPEN) this->closeDoors();
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
    switch (currentDoor) {
        case Elevator::DoorState::CLOSED:
            doorStr = "Closed.";
            break;
        case Elevator::DoorState::CLOSING:
            doorStr = "Closing...";
            break;
        case Elevator::DoorState::OPENING:
            doorStr = "Opening...";
            break;
        case Elevator::DoorState::OPEN:
            doorStr = "Open.";
            break;
        default:
            throw "ERROR: Invalid door state enum";
    }
    switch (currentEmergency) {
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
        case Elevator::EmergencyState::DOOR_OBSTACLE:
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

bool Elevator::isAtSafeFloor() const { return currentFloorNum == safeFloor; }

void Elevator::determineMovement() {
    updateEmergency();  // Update emergency state first

    int targetFloor;

    if (currentEmergency == EmergencyState::OVERLOAD) {
        // Cannot leave until overload is resolved
        targetFloor = currentFloorNum;
    } else if (currentEmergency == EmergencyState::FIRE ||
               currentEmergency == EmergencyState::POWER_OUT) {
        // Seek a safe floor, disregard queues.
        targetFloor = safeFloor;
    } else {
        // Collect floors that have their up/down floor buttons pressed,
        // or are targeted by this elevator's destination button panel.

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
        queuedFloors.erase(
            std::unique(queuedFloors.begin(), queuedFloors.end()),
            queuedFloors.end());

        // Compute optimal floor to move to
        targetFloor = closestQueuedFloor(queuedFloors);
    }

    if (currentFloorNum == targetFloor) {
        // Stop elevator on current floor.
        setMovement(MovementState::STOPPED);
        openDoors();
        destinationButtons[currentFloorNum]->setChecked(false);
        emit elevatorArrived();
    } else if (currentDoor == DoorState::CLOSED) {
        // Elevator needs to go to a target, and is able to move.
        if (currentFloorNum < targetFloor)
            setMovement(MovementState::UPWARDS);
        else if (currentFloorNum > targetFloor)
            setMovement(MovementState::DOWNWARDS);
    }
}

void Elevator::ring() { emit textOut(QString("*ring!*")); }

void Elevator::openDoors() {
    // Only attempt to open doors if the elevator is not moving
    if (isMoving()) return;

    switch (currentDoor) {
        case DoorState::CLOSED:
        case DoorState::CLOSING:
            // Start opening the doors and ring bell
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
    // Doors would already be closed if elevator is moving,
    // and doors should stay open in applicable emergency states
    if (isMoving() || currentEmergency == EmergencyState::OVERLOAD ||
        ((currentEmergency == EmergencyState::FIRE ||
          currentEmergency == EmergencyState::POWER_OUT) &&
         isAtSafeFloor()))
        return;

    switch (currentDoor) {
        case DoorState::OPEN:
        case DoorState::OPENING:
            // Start closing the doors and ring bell
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
    if (currentDoor != newDoorState) {
        currentDoor = newDoorState;

        // Obstacle button cannot be used when door is already closed.
        if (currentDoor == DoorState::CLOSED)
            obstacleButton->setDisabled(true);
        else
            obstacleButton->setDisabled(false);

        emit elevatorDataChanged();
    }
}

const QVector<int> Elevator::queuedDestinations() const {
    QVector<int> queued;

    for (auto i = destinationButtons.cbegin(), end = destinationButtons.cend();
         i != end; ++i) {
        if (i.value()->isChecked()) queued.append(i.key());
    }
    return queued;
}

int Elevator::closestQueuedFloor(const QVector<int> &floors) const {
    // This method should only be called on a nonempty list of floors.

    if (floors.isEmpty()) return currentFloorNum;  // Fallback

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

    /*
    Return the floor that is closer. In case of a tie, the floor that was
    on the direction the elevator was moving is prioritized. If the elevator
    had no direction it was moving in, the lower floor is prioritized.
    */
    if (distBefore < distAfter) {
        return closestBefore;
    } else if (distAfter < distBefore) {
        return closestAfter;
    } else {
        // Distance tied
        if (currentMovement == MovementState::UPWARDS)
            return closestAfter;  // Higher floor
        else
            return closestBefore;  // Lower floor
    }
}

QVector<QWidget *> Elevator::getDoorButtonWidgets() {
    return QVector<QWidget *>{qobject_cast<QWidget *>(openButton),
                              qobject_cast<QWidget *>(closeButton)};
}

QVector<QWidget *> Elevator::getEmergencyButtonWidgets() {
    return QVector<QWidget *>{qobject_cast<QWidget *>(overloadButton),
                              qobject_cast<QWidget *>(obstacleButton),
                              qobject_cast<QWidget *>(fireButton),
                              qobject_cast<QWidget *>(helpButton)};
}

QVector<QWidget *> Elevator::getDestButtonWidgets() {
    QVector<QWidget *> toReturn;

    for (auto i = destinationButtons.cbegin(), end = destinationButtons.cend();
         i != end; ++i)
        toReturn.append(qobject_cast<QWidget *>(i.value()));

    return toReturn;
}

const QString Elevator::getTextDisplay() const {
    // Emergencies take priority in display
    switch (currentEmergency) {
        case EmergencyState::HELP:
            return "HELP: (connecting to building safety service or 911...)";
        case EmergencyState::FIRE:
            if ((currentMovement == MovementState::STOPPED) && isAtSafeFloor())
                return "FIRE: Safe floor reached. Please disembark.";
            else
                return "FIRE: Moving to safe floor.";
        case EmergencyState::POWER_OUT:
            if ((currentMovement == MovementState::STOPPED) && isAtSafeFloor())
                return "POWER OUTAGE: Safe floor reached. Please disembark.";
            else
                return "POWER OUTAGE: Running on emergency power. Moving to "
                       "safe floor.";
        case EmergencyState::OVERLOAD:
            return "OVERLOAD: Please reduce the load.";
        case EmergencyState::DOOR_OBSTACLE:
            return "DOOR OBSTACLE: Please clear the doorway.";
        case EmergencyState::NONE:
        default:
            break;
    }

    // Display movement
    switch (currentMovement) {
        case MovementState::UPWARDS:
            return "▲ Going up...";
        case MovementState::DOWNWARDS:
            return "▼ Going down...";
        case MovementState::STOPPED:
            return "- Stopped.";
        default:
            break;
    }

    return "";
}

void Elevator::updateEmergency() {
    EmergencyState newState;

    // Earlier cases take priority when multiple are active.
    if (overloadButton->isChecked()) {
        // Overload has first priority, elevator cannot move when overloaded
        newState = EmergencyState::OVERLOAD;
    } else if (parentBuilding->buildingPowerOut()) {
        // Power out in building
        newState = EmergencyState::POWER_OUT;
    } else if (fireButton->isChecked() || parentBuilding->buildingOnFire()) {
        // Fire in elevator or building
        newState = EmergencyState::FIRE;
    } else if (doorCloseFailures >= doorCloseFailThreshold) {
        // Enough door close failures accumulated, start door obstacle state
        newState = EmergencyState::DOOR_OBSTACLE;
    } else if (helpButton->isChecked()) {
        // Help button pressed, connect to safety services or 911.
        newState = EmergencyState::HELP;
    } else {
        // No applicable emergencies
        newState = EmergencyState::NONE;
    }

    if (currentEmergency != newState) {
        currentEmergency = newState;

        // Audio warnings (using inline console instead of actual audio output)
        switch (currentEmergency) {
            case EmergencyState::FIRE:
                emit textOut(
                    "A fire has been detected. Moving towards safe floor.");
                break;
            case EmergencyState::POWER_OUT:
                emit textOut(
                    "A power outage has been detected. Moving towards safe "
                    "floor.");
                break;
            case EmergencyState::OVERLOAD:
                emit textOut("Overload. Please reduce the load.");
                break;
            case EmergencyState::DOOR_OBSTACLE:
                emit textOut(
                    "Door obstacle detected. Please clear the doorway.");
                break;
            case EmergencyState::HELP:
                emit textOut("(connected to building safety service or 911)");
                break;
            case EmergencyState::NONE:
            default:
                break;
        }

        emit elevatorDataChanged();
    }
}

bool Elevator::doorSensorSeesObstacle() const {
    return obstacleButton->isChecked();
}

const QBrush Elevator::getElevatorColor() const {
    switch (currentDoor) {
        case Elevator::DoorState::OPENING:
            return QBrush(Qt::darkGreen);
        case Elevator::DoorState::OPEN:
            return QBrush(Qt::green);
        case Elevator::DoorState::CLOSING:
            return QBrush(Qt::darkCyan);
        case Elevator::DoorState::CLOSED:
        default:
            return QBrush(Qt::cyan);
    }
}
