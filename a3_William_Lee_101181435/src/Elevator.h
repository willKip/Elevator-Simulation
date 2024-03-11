#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QMessageBox>
#include <QObject>
#include <QQueue>
#include <QSignalTransition>
#include <QState>
#include <QStateMachine>
#include <QString>

/**
 * Store and compute elevator movement and state.
 * They exist as part of Building classes.
 * Member variables:
 * - carId      Unique identifying ID of elevator within the building.
 * -
 */
class Elevator : public QObject {
    Q_OBJECT
   public:
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

    Elevator(int carId, QObject* parent = nullptr);

    // Unique ID given to each elevator within a building.
    const int carId;

    // Ordered FIFO queue of floors selected on the floor panel.
    QQueue<int> pressedFloors;

    MovementState currentMovement;
    DoorState doorState;
    EmergencyState emergencyState;

    // Return a string representing the elevator's status.
    QString getElevatorString() const;

   signals:
    // Fired when an elevator has changed to a moving state.
    void elevatorMoving();

   signals:
    // Fired when an elevator arrives at a destination.
    void elevatorArrived();

   public slots:
    // Called by the building every time there is a change to the building state
    void determineMovement();

   private:
    // todo: current text message and audio
    // If current floor has queue, stop and open for it.
};

#endif /* ELEVATOR_H */
