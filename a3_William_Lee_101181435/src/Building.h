#ifndef BUILDING_H
#define BUILDING_H

#include <QAbstractTableModel>
#include <QMap>
#include <QVector>

#include "Direction.h"

// Forward declarations
class Elevator;
class DataButton;
struct floorData;

/** Simulates a building with elevators.
 *
 * Extends QAbstractTableModel, serving the role of Model in the MVC paradigm.
 * The role of the building in the simulation, holding all the floors and
 * elevators and providing a table-like interface to access through a Qt view.
 *
 * Data Members:
 * + floorData: struct
 *      Data struct to hold pointers to each floor's directional buttons.
 * + floorCount: int
 *      Number of floors in the building. Floors correspond to rows.
 * + elevatorCount: int
 *      Number of elevators in the building. Elevators correspond to columns.
 * + rowButtonCount: int
 *      Number of additional rows allotted for buttons
 * + colButtonCount: int
 *      Number of additional columns allotted for buttons
 *
 * - floorNum_FloorData_Map: QMap<int, floorData>
 * - carId_Elevator_Map: QMap<int, Elevator *>
 *      Ascending order mappings of floor numbers and elevator IDs to their
 *      corresponding floorData structs and Elevator pointers.
 *
 * - buildingFireButton: DataButton *
 * - buildingPowerOutButton: DataButton *
 *      Buttons for toggling simulated building-wide emergencies.
 *
 * - floorButtonUiWidth: int
 *      Defines the maximum width of the floor buttons in the UI.
 *
 * Class Methods:
 * + index_to_floorNum(int): int
 * + index_to_carId(int): int
 *      Defines the relationship between data indices used in this class and
 *      floor numbers / elevator car IDs, and returns the converted numbers.
 *
 * + getQueuedFloors(Direction): QVector<int>
 *      Returns a list of floor numbers where the buttons active on the floor
 *      match the direction given. If no direction (Direction::NONE) is given,
 *      return all floors with any direction active.
 *
 * + buildingOnFire(): bool
 * + buildingPowerOut(): bool
 *      Returns true if the related emergency is active in the building.
 *
 * + getEmergencyButtons(): QVector<QWidget *>
 *      Return Qt widget pointers to the emergency simulation buttons of the
 *      building.
 * + getFloorButtons_byIndex(int): QVector<QWidget *>
 *      Return Qt widget pointers to the floor buttons of the floor with a
 *      matching index.
 *
 * + getFloorData_byIndex(int): Floor*
 * + getFloorData_byFloorNum(int): Floor*
 * + getElevator_byIndex(int): Elevator*
 * + getElevator_byCarId(int): Elevator*
 *      Getters to retrieve floorData structs and Elevator objects by either
 *      their Building data indices or assigned floor numbers/elevator car IDs.
 *
 * + Implementations of virtual functions from QAbstractTableModel
 *
 * - getElevator_byIndex(int) const: const Elevator *
 *      const type getter method needed in data(). Retrieves a constant
 *      version of the elevator by index for updating the view.
 *
 * - isFloorDataIndex(int): bool
 * - isElevatorIndex(int): bool
 *      Returns true if index accesses are within the floor/elevator bounds
 *
 * - validateFloorDataIndex(int): void
 * - validateElevatorIndex(int): void
 *      Throws an exception if the index access is not on a valid
 *      floor/elevator bound.
 *
 * - updateColumn(int): void
 *      Updates the column specified to reflect data changes in the view.
 *
 * Signals:
 * + buildingDataChanged(): void
 *      Emitted when there is a change to data in the building.
 *      (e.g. Floor button pressed, elevator position changed)
 */
class Building : public QAbstractTableModel {
    Q_OBJECT

   public:
    Building(int floorCount, int elevatorCount, int rowButtonCount = 0,
             int colButtonCount = 0, QObject *parent = nullptr);

    /* Public data structs */
    typedef struct floorData {
        DataButton *upButton;
        DataButton *downButton;

        floorData() : upButton(nullptr), downButton(nullptr) {}
        floorData(DataButton *up, DataButton *down)
            : upButton(up), downButton(down) {}
    } floorData;

    /* Public data members */
    const int floorCount;
    const int elevatorCount;
    const int rowButtonCount;
    const int colButtonCount;

    /* Public methods */
    int index_to_floorNum(int) const;
    int index_to_carId(int) const;

    const QVector<int> getQueuedFloors(Direction = Direction::NONE) const;

    bool buildingOnFire() const;
    bool buildingPowerOut() const;

    QVector<QWidget *> getEmergencyButtons();
    QVector<QWidget *> getFloorButtons_byIndex(int);

    floorData getFloorData_byIndex(int);
    floorData getFloorData_byFloorNum(int);

    Elevator *getElevator_byIndex(int);
    Elevator *getElevator_byCarId(int);

    /* Implemented virtual functions from QAbstractTableModel */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation, int role) const override;

   signals:
    void buildingDataChanged();

   private:
    /* Private data members */
    QMap<int, floorData> floorNum_FloorData_Map;
    QMap<int, Elevator *> carId_Elevator_Map;

    DataButton *const buildingFireButton;
    DataButton *const buildingPowerOutButton;

    static const int floorButtonUiWidth = 70;

    /* Private methods */
    const Elevator *getElevator_byIndex(int) const;

    bool isFloorDataIndex(int) const;
    bool isElevatorIndex(int) const;

    void validateFloorDataIndex(int) const;
    void validateElevatorIndex(int) const;

    void updateColumn(int);
};

#endif /* BUILDING_H */
