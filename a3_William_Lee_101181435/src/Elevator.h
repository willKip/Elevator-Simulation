#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>

#include "DataButton.h"
#include "DestButton.h"

class Building;

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

    Elevator(int buildingColIndex, int carId, int initialFloorNum,
             Building *parentBuilding, QObject *parent = nullptr);

    const int buildingColIndex;  // Data index within a Building
    const int carId;             // Unique elevator car ID within a Building
    int currentFloorNum;         // Current floor of the elevator

    // Buttons to override door states
    DataButton *const openButton;
    DataButton *const closeButton;

    // Destination buttons; mapping floor numbers to their button pointers.
    QMap<int, DestButton *> destinationButtons;

    // Return a string representing the elevator's status.
    const QString getElevatorString() const;

    // Return true if the elevator is currently moving.
    bool isMoving() const;

   signals:
    // Fired when an aspect of the elevator has changed.
    void elevatorDataChanged(int index);

   public slots:
    // Called by the building every time there is a change to the building state
    void determineMovement();

    // Attempt to open or close the doors
    void openDoors();
    void closeDoors();

   private:
    // Pointer to Building Elevator exists in
    Building *const parentBuilding;

    MovementState currentMovement;
    DoorState doorState;
    EmergencyState emergencyState;

    // Movement timer for each elevator, simulates moving speed.
    // Timer simulating how fast the elevator's doors close or open.
    // Timer for doors to stay open before automatically closing.

    /**
     * Timers and their interval definitions
     */
    QTimer *const movementTimer;
    QTimer *const doorSpeedTimer;
    QTimer *const doorWaitTimer;

    // How long it takes for an elevator to reach a new floor in the simulation
    static const int movementMs = 1000;
    // How long it takes for a door to fully close
    static const int doorSpeedMs = 1000;
    // How long it takes before an open door will close itself
    static const int doorWaitMs = 2000;

    // Private setters to emit signals appropriately upon data change
    void setMovement(MovementState);
    void setDoorState(DoorState);

    void moveElevator();

    // Return a list of floors queued on the elevator panel.
    const QVector<int> queuedDestinations() const;

    // From a given nonempty list of floors, returns the number of the ideal
    // next floor to visit.
    int closestQueuedFloor(const QVector<int> &floors) const;

    // TODO: current text message and audio
    // TODO: If current floor has queue, stop and open for it.
};

#endif /* ELEVATOR_H */
