#include "Elevator.h"

#include <QObject>
#include <unordered_map>

Elevator::Elevator(int carId, int floorCount, QObject *parent)
    : QObject(parent),
      currentMovement(MovementState::STOPPED),
      doorState(DoorState::CLOSED) {}

// int Elevator::readDoorSensor() {
//     return
//  }