#ifndef BUILDING_H
#define BUILDING_H

#include <QAbstractTableModel>
#include <QHash>
#include <QMap>
#include <QVector>

#include "Direction.h"

// Forward declarations
class Elevator;
class Floor;

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
    Floor *getFloor_byIndex(int index);
    Floor *getFloor_byFloorNum(int floorNum);
    Elevator *getElevator_byIndex(int index);
    Elevator *getElevator_byCarId(int carId);

    // Defines how floor and elevator IDs are assigned relative to their
    // indices.
    int index_to_floorNum(int index) const;
    int index_to_carId(int index) const;

    // Get an ascending list of floor numbers where the floors have pressed
    // buttons (pending elevator requests). If a direction is specified, return
    // only the floors that have the matching direction's buttons pressed.
    const QVector<int> getQueuedFloors(Direction = Direction::NONE) const;

    // Update a specific column (elevator) in the view.
    void updateColumn(int col);

   signals:
    // Fired when there is a change to building data (e.g. floor button pressed,
    // elevator at new floor)
    void buildingDataChanged();

   private:
    // Const access method for data function use
    const Elevator *getElevator_byIndex(int index) const;

    // Returns true if index accesses are on the floor/elevator data.
    bool isFloorDataIndex(int index) const;
    bool isElevatorIndex(int index) const;

    // Throws exception if index access would be outside the floor/elevator data
    // range.
    void validateFloorDataIndex(int index) const;
    void validateElevatorIndex(int index) const;

    /**
     * Ascending order mappings of floor numbers and elevator IDs to
     * corresponding floor and elevator data classes.
     */
    QMap<int, Floor *> floorNum_FloorData_Map;
    QMap<int, Elevator *> carId_Elevator_Map;

    // Maps of building object table indices to floor numbers and elevator IDs.
    QHash<int, int> index_floorNum_Map;
    QHash<int, int> index_carId_Map;
};
#endif /* BUILDING_H */
