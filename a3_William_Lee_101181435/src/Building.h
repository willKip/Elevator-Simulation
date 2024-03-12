#ifndef BUILDING_H
#define BUILDING_H

#include <QAbstractTableModel>
#include <QBrush>
#include <QDebug>
#include <QFont>
#include <QHash>
#include <QMap>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QVector>

#include "Direction.h"
#include "FloorButton.h"

class Elevator;  // Forward declare Elevator class

// Helper data classes for better management of associated data
class ElevatorData;
class FloorData;

// Main class
class Building;

// TODO documentation
/**
 * Class designed to couple elevator-related data on the building side.
 */
class ElevatorData : public QObject {
    Q_OBJECT
   public:
    ElevatorData(int index, int carId, int initFloorNum,
                 Building *parentBuilding, QObject *parent = nullptr);

    const int index;      // Data index within a Building
    const int carId;      // Unique elevator car ID within a Building
    int currentFloorNum;  // Current floor of the elevator

    // Invoked when movement timer expires, reflects movement on building.
    void moveElevator();

    // Return display string for elevator data
    const QString getDisplayString() const;

   private:
    // Pointer to Elevator object
    Elevator *const obj;

    // Pointer to Building ElevatorData belongs to
    Building *const parentBuilding;

    // Movement timer for each elevator, simulates moving speed.
    QTimer *const movementTimer;

    // Timer for door opening and closing.
    QTimer *const doorTimer;

    // How long it takes for an elevator to reach a new floor in the simulation.
    static const int movementMs = 1000;
    // How long it takes for a door to fully close, in milliseconds.
    static const int doorMs = 3000;
};

// TODO: documentation
class FloorData : public QObject {
    Q_OBJECT
   public:
    FloorData(int index, int floorNumber, Building *parentBuilding,
              QObject *parent = nullptr);

    const int index;        // Data index within a Building
    const int floorNumber;  // Unique floor number within a Building

    // Directional buttons of the floor. Public so that they can be accessed for
    // adding to the UI.
    FloorButton *const upButton;
    FloorButton *const downButton;

    // Return checked state of floor buttons
    bool pressedUp() const;
    bool pressedDown() const;

    // Set all buttons of the floor to unchecked
    void resetButtons();

   private:
    Building *const parentBuilding;  // Pointer to Building FloorData belongs to
};

/**
 * Class simulating a building with elevators.
 * In charge of storing and altering elevator locations and floor button states
 * according to requests.
 * Extends QAbstractTableModel, serving the role of Model in the MVC paradigm.
 *
 * Member variables:
 * - floorCount        Number of floors in the building.
 * - elevatorCount     Number of elevators in the building.
 * - buildingTable      2-dimensional matrix with rows as floors, columns as
 *                      elevators.
 */
// TODO: description
class Building : public QAbstractTableModel {
    Q_OBJECT

   public:
    enum class EmergencyState { FIRE, POWER_OUT };

    Building(int floorCount, int elevatorCount, int rowButtonCount = 0,
             int colButtonCount = 0, QObject *parent = nullptr);

    const int floorCount;     // Each floor gets a row
    const int elevatorCount;  // Each elevator gets a column

    // Additional non-data rows and columns allocated for buttons.
    const int rowButtonCount;
    const int colButtonCount;

    // Implement virtual functions from QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation, int role) const override;

    // Access methods for floor and elevator data
    FloorData *getFloor_byIndex(int index);
    FloorData *getFloor_byFloorNum(int floorNum);
    ElevatorData *getElevator_byIndex(int index);
    ElevatorData *getElevator_byCarId(int carId);

    // Get an ascending list of floor numbers where the floors have pressed
    // buttons (pending elevator requests). If a direction is specified, return
    // only the floors that have the matching direction's buttons pressed.
    const QVector<int> getQueuedFloors(Direction = Direction::NONE) const;

    /**
     * Set the elevator of the given ID to the given floor.
     * Returns the previous floor number.
     */
    int placeElevator(int carId, int newFloorNum);

    // Update a specific column (elevator) in the view.
    void updateColumn(int col);

   signals:
    // Fired when there is a change to building data (e.g. floor button pressed,
    // elevator at new floor)
    void buildingDataChanged();

   private:
    // Defines how floor and elevator IDs are assigned relative to their
    // indices.
    int index_to_floorNum(int index) const;
    int index_to_carId(int index) const;
    // Const access method for data function use
    const ElevatorData *getElevator_byIndex(int index) const;

    // Returns true if index accesses are on the floor/elevator data.
    bool isFloorDataIndex(int index) const;
    bool isElevatorDataIndex(int index) const;

    // Throws exception if index access would be outside the floor/elevator data
    // range.
    void validateFloorDataIndex(int index) const;
    void validateElevatorDataIndex(int index) const;

    /**
     * Ascending order mappings of floor numbers and elevator IDs to
     * corresponding floor and elevator data classes.
     */
    QMap<int, FloorData *> floorNum_FloorData_Map;
    QMap<int, ElevatorData *> carId_ElevatorData_Map;

    // Maps of building object table indices to floor numbers and elevator IDs.
    QHash<int, int> index_floorNum_Map;
    QHash<int, int> index_carId_Map;
};
#endif /* BUILDING_H */
