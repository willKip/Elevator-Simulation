#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QObject>
#include <unordered_map>
#include <QStateMachine>

/**
 * Store and compute elevator movement and state.
 * Member variables:
 * - carId      Unique identifying ID of elevator within the building.
 * -
 */
class Elevator : public QObject {
    Q_OBJECT
   public:
    Elevator(int carId, int floorCount, QObject *parent = nullptr);

    enum class MovementState { STOPPED, UPWARDS, DOWNWARDS };
    enum class DoorState { CLOSED, CLOSING, OPENING, OPEN };
    enum class EmergencyState { FIRE, POWER_OUT, OVERLOAD, DOOR_STUCK, HELP };

    int readDoorSensor();

   private:
    /**
     * Map of floor numbers to booleans. Value of true means the floor needs to
     * be visited by the elevator (during normal operation).
     * Nonexistent accesses default to false, so no initialization required.
     */
    std::unordered_map<int, bool> queuedFloors;
    MovementState currentMovement;
    DoorState doorState;

    // todo: determine next action
    // If current floor has queue, stop and open for it.
    //
    // todo: door opening should take 2 intervals

    // signals:
    // Notify building that elevator moving.
    // slots:
    // Receive from floor sensor current floor.
};

#endif /* ELEVATOR_H */
