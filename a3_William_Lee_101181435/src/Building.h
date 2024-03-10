#ifndef BUILDING_H
#define BUILDING_H

#include <QAbstractTableModel>
#include <QBrush>
#include <QDebug>
#include <QFont>
#include <QHash>
#include <QMutex>
#include <QPushButton>
#include <QString>
#include <QVector>

#include "Elevator.h"

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
 * TODO
 */
class Building : public QAbstractTableModel {
    Q_OBJECT

   public:
    enum class EmergencyState { FIRE, POWER_OUT };
    enum class Direction { UP, DOWN };

    // Nested data classes for ease of organization
    class FloorData {
       public:
        FloorData(int i, int fn) : index(i), floorNumber(fn){};

        const int index;
        const int floorNumber;
        bool pressedUp() const { return upButton->isChecked(); }
        bool pressedDown() const { return downButton->isChecked(); }

        // Will be assigned by MainWindow
        QPushButton *upButton;
        QPushButton *downButton;
    };

    class ElevatorData {
       public:
        ElevatorData(int index, int carId, int initFloorNum,
                     Building *parentBuilding)
            : index(index),
              carId(carId),
              currentFloorNum(initFloorNum),
              obj(new Elevator(carId, parentBuilding)) {
            // Connect signals between building and new elevator
            connect(obj, &Elevator::movingStateSig, parentBuilding,
                    &Building::moveElevator);
            connect(parentBuilding, &QAbstractItemModel::dataChanged, obj,
                    &Elevator::updateBuildingData);
        };

        const int index;
        const int carId;
        int currentFloorNum;
        Elevator *const obj;
    };

    Building(int floorCount, int elevatorCount, int buttonRowCount = 0,
             int buttonColCount = 0, QObject *parent = nullptr);

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

   public slots:
    // Reflect floor button state changes on data
    void updateFloorRequests();

   private slots:
    // Move elevator one floor in the specified direction.
    void moveElevator(int carId, Elevator::MovementState elevatorDirection);

   private:
    QMutex mutex;

    const int floorCount;     // Each floor gets a row
    const int elevatorCount;  // Each elevator gets a column

    // Additional non-data rows and columns can be allocated for buttons.
    const int buttonRowCount;
    const int buttonColCount;

    // Defines how floor and elevator IDs are assigned relative to their
    // indices.
    int index_to_floorNum(int index) const;
    int index_to_carId(int index) const;
    // Const access method for data function use
    const ElevatorData *getElevator_byIndex(int index) const;

    // Mappings of data indices to corresponding floor and elevator data
    // classes. Use access methods to manipulate after initialization.
    QHash<int, FloorData *> indexFloorMap;
    QHash<int, ElevatorData *> indexElevatorMap;
    QHash<int, int> floorNum_toIndexMap;
    QHash<int, int> carId_toIndexMap;

    /**
     * Set the elevator of the given ID to the given floor.
     * Returns the previous floor number.
     */
    int placeElevator(int carId, int newFloorNum);
};
#endif /* BUILDING_H */
