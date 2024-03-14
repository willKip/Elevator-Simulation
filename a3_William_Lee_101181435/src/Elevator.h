#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QBrush>
#include <QMap>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>
#include <QWidget>

// Forward declarations
class DataButton;
class Building;

/** Store and compute elevator movement and state.
 *
 * Decentralized elevator class. Receives signals from the parent building
 * notifying of an update to the building state, computes movement and
 * emergency state, and notifies of change in its status to the building.
 *
 * Enums:
 * - MovementState
 *      Whether the elevator is moving, and to which direction.
 * - DoorState
 *      What state the elevator's doors can be in.
 * - EmergencyState
 *      The exceptional states the elevator can be in.
 *
 * Data Members:
 * - parentBuilding: Building *
 *      Pointer to the Building that Elevator exists in.
 *
 * - openButton: DataButton *
 * - closeButton: DataButton *
 * - fireButton: DataButton *
 * - obstacleButton: DataButton *
 * - helpButton: DataButton *
 * - overloadButton: DataButton *
 *      Pointers to door override and emergency simulation buttons.
 *
 * - destinationButtons: QMap<int, DataButton *>
 *      Mapping of floor numbers to their corresponding buttons on the elevator
 *      destination panel.
 *
 * - currentMovement: MovementState
 *      Current elevator movement.
 * - currentDoor: DoorState
 *      Current door state.
 * - currentEmergency: EmergencyState
 *      Current emergency state. Elevator can be in one emergency state, with
 *      implementation-dependent priorities.
 *
 * - movementTimer: QTimer *
 *      Timer for simulating the speed at which an elevator reaches a new floor.
 * - doorSpeedTimer: QTimer *
 *      Timer for simulating the speed at which a door will fully open or close.
 * - doorWaitTimer: QTimer *
 *      Timer for simulating the time an elevator's doors will stay open.
 * - movementMs: int
 *      How long it takes for an elevator to reach a new floor, in milliseconds.
 * - doorSpeedMs: int
 *      How long it takes for the doors to open or close fully, in milliseconds.
 * - doorWaitMs: int
 *      How long an open elevator will wait until closing the doors.
 *
 * - doorCloseFailures: int
 *      Number of failed attempts to close the door, incremented when the door
 *      sensors detect an obstacle, reset to zero when the door successfully
 *      closes.
 * - doorCloseFailThreshold: int
 *      Max number of failed door close attempts before the elevator will alert
 *      passengers of a door obstacle.
 *
 * - safeFloor: int
 *      Safe floor an elevator should head to in an applicable emergency.
 *
 * + currentFloorNum: int
 *      The number of the floor the elevator is currently at.
 *
 * Class Methods:
 * - setMovement(MovementState): void
 * - setDoorState(DoorState): void
 *      Private setters that will emit signals or trigger responses on data
 *      change.
 *
 * - updateEmergency(): void
 *      Checks the relevant data for applicable elevator emergency states at
 *      that moment, and applies it to the elevator.
 *
 * - doorSensorSeesObstacle(): bool
 *      Returns true if the elevator light sensors detect an obstacle.
 *
 * - ring(): void
 *      Rings the bell of the elevator.
 *
 * - queuedDestinations(): QVector<int>
 *      Returns a list of floors that have active buttons on the panel.
 *
 * - closestQueuedFloor(QVector<int>): int
 *      From a supplied nonempty list of floor numbers, computes and returns the
 *      ideal floor to visit next from the elevator's current floor.
 *
 * - isMoving(): bool
 *      Returns true if the elevator is currently moving.
 *
 * - isAtSafeFloor(): bool
 *      Returns true if the elevator is currently at a safe floor.
 *
 * + getElevatorString(): QString
 *      Returns a string representing the elevator's current status.
 *
 * + getTextDisplay(): QString
 *      Returns a string to display in the elevator's display panel.
 *
 * + getDoorButtonWidgets(): QVector<QWidget *>
 * + getDestButtonWidgets(): QVector<QWidget *>
 * + getEmergencyButtonWidgets(): QVector<QWidget *>
 *      Getters returning QWidget pointers of the elevator's buttons, for use in
 *      adding them to the UI in MainWindow.
 *
 * + getElevatorColor(): QBrush
 *      Returns the appropriate background colour for the elevator in the view.
 *
 * Signals:
 * + textOut(const QString &): void
 *      Emitted to display text in the UI. Captured by MainWindow.
 *
 * Slots:
 * - determineMovement(): void
 *      Examine the current elevator data and compute next movement.
 *
 * - openDoors(): void
 * - closeDoors(): void
 *      Opens or closes the elevator's doors if it is currently possible to.
 */
class Elevator : public QObject {
    Q_OBJECT

   private:
    /* Private enums */
    enum class MovementState { STOPPED, UPWARDS, DOWNWARDS };
    enum class DoorState { CLOSED, CLOSING, OPENING, OPEN };
    enum class EmergencyState {
        NONE,
        FIRE,
        POWER_OUT,
        OVERLOAD,
        DOOR_OBSTACLE,
        HELP
    };

    /* Private data members */
    Building *const parentBuilding;

    DataButton *const openButton;
    DataButton *const closeButton;

    DataButton *const fireButton;
    DataButton *const obstacleButton;
    DataButton *const helpButton;
    DataButton *const overloadButton;

    QMap<int, DataButton *> destinationButtons;

    MovementState currentMovement;
    DoorState currentDoor;
    EmergencyState currentEmergency;

    QTimer *const movementTimer;
    QTimer *const doorSpeedTimer;
    QTimer *const doorWaitTimer;

    static const int movementMs = 1000;  // 1 second
    static const int doorSpeedMs = 800;  // 0.8 seconds
    static const int doorWaitMs = 1500;  // 1.5 seconds

    int doorCloseFailures;

    static const int doorCloseFailThreshold = 3;

    static const int safeFloor = 1;

    /* Private methods */
    void setMovement(MovementState);
    void setDoorState(DoorState);

    void updateEmergency();

    bool doorSensorSeesObstacle() const;

    void ring();

    const QVector<int> queuedDestinations() const;

    int closestQueuedFloor(const QVector<int> &floors) const;

    bool isMoving() const;
    bool isAtSafeFloor() const;

   public:
    Elevator(int initialFloorNum, Building *parentBuilding,
             QObject *parent = nullptr);

    /* Public data members */
    int currentFloorNum;

    /* Public methods */
    const QString getElevatorString() const;
    const QString getTextDisplay() const;

    QVector<QWidget *> getDoorButtonWidgets();
    QVector<QWidget *> getDestButtonWidgets();
    QVector<QWidget *> getEmergencyButtonWidgets();

    const QBrush getElevatorColor() const;

   signals:
    // Fired when an aspect of the elevator has changed.
    void elevatorDataChanged();

   signals:
    // Fired when an elevator has stopped at a location to take passengers.
    void elevatorArrived();

   signals:
    void textOut(const QString &);

   private slots:
    void determineMovement();

    // Attempt to open or close the doors
    void openDoors();
    void closeDoors();
};

#endif /* ELEVATOR_H */
