#include "Elevator.h"

#include <QMessageBox>
#include <QObject>
#include <QQueue>
#include <QSignalTransition>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QTimer>
#include <algorithm>

#include "Building.h"
#include "Direction.h"

Elevator::Elevator(int carId, QObject *parent)
    : QObject(parent),
      carId(carId),
      currentMovement(MovementState::STOPPED),
      doorState(DoorState::CLOSED),
      emergencyState(EmergencyState::NONE) {}

void Elevator::determineMovement() {
    Building *building = qobject_cast<Building *>(sender());
    ElevatorData *elevatorData = building->getElevator_byCarId(carId);
    int currFloorNum = elevatorData->currentFloorNum;

    // From a given nonempty list of floors, returns the number of the floor
    // closest to the current floor. If the floor is equal distance between two
    // floors, prioritizes the lower floor.
    auto closestQueuedFloor =
        [currFloorNum](const QVector<int> &floors) -> int {
        if (floors.isEmpty())
            throw "ERROR: No floors are queued, cannot find closest queued";
        // TODO: ^, whether elevator was moving or idle determines whether it
        // opens door or not.

        // Current floor is below or above all queued floors.
        if (currFloorNum <= floors.first()) return floors.first();
        if (currFloorNum >= floors.last()) return floors.last();

        // Current floor is between two queued floors, before and after.
        const int *closestIt = std::adjacent_find(
            floors.begin(), floors.end(),
            [currFloorNum](int before, int after) {
                return currFloorNum >= before && currFloorNum <= after;
            });

        int closestBefore = *closestIt;       // Closest queued before current
        int closestAfter = *(closestIt + 1);  // Closest queued after current

        // Distances to each floor.
        int distBefore = currFloorNum - closestBefore;
        int distAfter = closestAfter - currFloorNum;

        // Return floor that is closer. In case of a tie, lower floor first.
        if (distBefore <= distAfter)
            return closestBefore;
        else
            return closestAfter;
    };

    // No buttons pressed on elevator's panel.
    // Go to closest floor waiting for an elevator, any direction.
    QVector<int> queuedFloors = building->getQueuedFloors();
    // No eligible floors queued
    if (queuedFloors.isEmpty()) {
        currentMovement = MovementState::STOPPED;
        emit elevatorArrived();
        return;
    }
    int closestFloor = closestQueuedFloor(queuedFloors);

    if (currFloorNum == closestFloor) {
        currentMovement = MovementState::STOPPED;
        emit elevatorArrived();
    } else if (currFloorNum < closestFloor) {
        if (currentMovement != MovementState::UPWARDS) {
            currentMovement = MovementState::UPWARDS;
            emit elevatorMoving();
        }
    } else if (currFloorNum > closestFloor) {
        if (currentMovement != MovementState::DOWNWARDS) {
            currentMovement = MovementState::DOWNWARDS;
            emit elevatorMoving();
        }
    }
}

QString Elevator::getElevatorString() const {
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
