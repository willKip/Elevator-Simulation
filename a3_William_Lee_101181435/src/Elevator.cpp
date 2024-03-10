#include "Elevator.h"

#include <QMessageBox>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QSignalTransition>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QTimer>

#include "Building.h"

Elevator::Elevator(int carId, QObject *parent)
    : QObject(parent),
      carId(carId),
      currentMovement(MovementState::STOPPED),
      doorState(DoorState::CLOSED),
      emergencyState(EmergencyState::NONE) {}

void Elevator::determineMovement() {
    // Slot called by the building every time theres a change

    using MS = MovementState;

    ElevatorData *elevatorData =
        qobject_cast<Building *>(sender())->getElevator_byCarId(carId);

    MS newMovement;

    if (elevatorData->currentFloorNum < 7) {
        newMovement = MS::UPWARDS;
    } else if (elevatorData->currentFloorNum == 7) {
        newMovement = MS::DOWNWARDS;
    }

    if (currentMovement != newMovement) {
        currentMovement = newMovement;
        elevatorData->startMovement();
    }
}

QString Elevator::getElevatorString() const {
    QString movementStr;
    QString doorStr;
    QString emergencyStr;

    switch (currentMovement) {
        case Elevator::MovementState::STOPPED:
            movementStr = "-";
            break;
        case Elevator::MovementState::UPWARDS:
            movementStr = "↑";
            break;
        case Elevator::MovementState::DOWNWARDS:
            movementStr = "↓";
            break;
        default:
            throw "ERROR: Invalid Movement enum";
    }
    switch (doorState) {
        case Elevator::DoorState::CLOSED:
            doorStr = "(closed)";
            break;
        case Elevator::DoorState::CLOSING:
            doorStr = "(closing)";
            break;
        case Elevator::DoorState::OPENING:
            doorStr = "(opening)";
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
