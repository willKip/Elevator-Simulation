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

    Elevator(int carId, QObject *parent = nullptr);

    // Unique ID given to each elevator within a building.
    const int carId;

    MovementState getMovementState() const;

    // Return a string representing the elevator's status.
    QString getElevatorString() const;

   signals:
    // Fired only when an elevator's movement state has changed from before.
    void elevatorMovementChanged();

   signals:
    // Fired when an elevator arrives at a destination.
    void elevatorArrived();

   public slots:
    // Called by the building every time there is a change to the building state
    void determineMovement();

   private:
    // Ordered FIFO queue of floors selected on the floor panel.
    QQueue<int> pressedFloors;

    MovementState currentMovement;
    DoorState doorState;
    EmergencyState emergencyState;

    // Timer simulating how fast the elevator's doors close or open.
    QTimer *const doorTimer;

    // Timer for doors to stay open before automatically closing.
    QTimer *const doorTimeoutTimer;

    // How long it takes for a door to fully close, in milliseconds.
    static const int doorMs = 3000;
    // How long it takes before an open door will close itself.
    static const int doorTimeoutMs = 10000;

    // Update elevator movement state, broadcast change if it is different from
    // before.
    void updateMovementState(MovementState);

    // TODO: current text message and audio
    // TODO: If current floor has queue, stop and open for it.
};

#endif /* ELEVATOR_H */
