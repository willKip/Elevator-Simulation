#include "Elevator.h"

#include <QObject>
#include <QQueue>
#include <QString>

Elevator::Elevator(int carId, int initialFloor, QObject *parent)
    : QObject(parent),
      carId(carId),
      currentMovement(MovementState::STOPPED),
      doorState(DoorState::CLOSED),
      emergencyState(EmergencyState::NONE),
      currentFloor(initialFloor) {}

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
int Elevator::getCarId() const { return carId; }
int Elevator::getCurrentFloor() const { return currentFloor; }
void Elevator::setCurrentFloor(int newFloor) { currentFloor = newFloor; }

// int Elevator::readDoorSensor() { return }
