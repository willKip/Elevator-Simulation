#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector>
#include <QWidget>

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

    /**
     * Public data members
     */
    const int buildingColIndex;  // Data index within a Building
    const int carId;             // Unique elevator car ID within a Building
    int currentFloorNum;         // Current floor of the elevator

    /**
     * Public methods
     */
    // Return a string representing the elevator's status.
    const QString getElevatorString() const;

    // Return true if the elevator is currently moving.
    bool isMoving() const;

    // Return QWidget pointers to elevator's buttons for adding to UI
    QVector<QWidget *> getDoorButtonWidgets();
    QVector<QWidget *> getDestButtonWidgets();

   signals:
    // Fired when an aspect of the elevator has changed.
    void elevatorDataChanged();

   signals:
    // Fired when an elevator has stopped at a location to take passengers.
    void elevatorArrived();

   public slots:
    // Called by the building every time there is a change to the building state
    void determineMovement();

    // Attempt to open or close the doors
    void openDoors();
    void closeDoors();

   private:
    /**
     * Private data members
     */
    // Pointer to Building Elevator exists in
    Building *const parentBuilding;

    // Buttons to override door states
    DataButton *const openButton;
    DataButton *const closeButton;

    // Destination buttons; mapping floor numbers to their button pointers.
    QMap<int, DestButton *> destinationButtons;

    MovementState currentMovement;
    DoorState doorState;
    EmergencyState emergencyState;

    QTimer *const movementTimer;   // Simulates elevator movement
    QTimer *const doorSpeedTimer;  // Simulates door opening/closing
    QTimer *const doorWaitTimer;   // Simulates door timeout before auto close

    // How long it takes for an elevator to reach a new floor in the simulation
    static const int movementMs = 1000;
    // How long it takes for a door to fully open or close
    static const int doorSpeedMs = 1000;
    // How long it takes before an open elevator will close itself
    static const int doorWaitMs = 2000;

    /**
     * Private methods
     */
    // Private setters that emit signals appropriately on data change
    void setMovement(MovementState);
    void setDoorState(DoorState);

    // Return a list of floors queued on the elevator panel.
    const QVector<int> queuedDestinations() const;

    // From a given nonempty list of floors, returns the number of the ideal
    // next floor to visit.
    int closestQueuedFloor(const QVector<int> &floors) const;

    // TODO: current text message and audio
    // TODO: If current floor has queue, stop and open for it.
};

#endif /* ELEVATOR_H */
