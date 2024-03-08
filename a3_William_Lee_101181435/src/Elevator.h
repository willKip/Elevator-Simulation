#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QObject>
#include <QQueue>
#include <QStateMachine>
#include <QString>

#include "Direction.h"

/**
 * Store and compute elevator movement and state.
 * Member variables:
 * - carId      Unique identifying ID of elevator within the building.
 * -
 */
class Elevator : public QObject {
    Q_OBJECT
   public:
    Elevator(int carId, int initialFloor, QObject *parent = nullptr);

    enum class MovementState { STOPPED, UPWARDS, DOWNWARDS };
    enum class DoorState { CLOSED, CLOSING, OPENING, OPEN };
    enum class EmergencyState {
        NONE,
        FIRE,
        POWER_OUT,
        OVERLOAD,
        DOOR_STUCK,
        HELP
    };

    // int readDoorSensor();

    // Getters
    int getCarId() const;
    int getCurrentFloor() const;

    // Setters
    void setCurrentFloor(int f);

    // Return a string representing the elevator's status.
    QString getElevatorString() const;

   signals:
    void elevatorMoving(Direction d);

   private:
    const int carId;
    // Ordered FIFO queue of floors selected on the floor panel.
    QQueue<int> queuedFloors;
    MovementState currentMovement;
    DoorState doorState;
    EmergencyState emergencyState;

    int currentFloor;

    // todo: current text message and audio

    // todo: determine next action
    // If current floor has queue, stop and open for it.
    //
    // todo: door opening should take 2 intervals

    // fn: query for floors that need an elevator. (remove if they no longer
    // need.)

    // signals:
    // Notify building that elevator moving.
    // retrieve current building status.
    // slots:
    // Receive from floor sensor current floor.
};

#endif /* ELEVATOR_H */
