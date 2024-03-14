#include "Building.h"

#include <QAbstractTableModel>
#include <QBrush>
#include <QMap>
#include <QRandomGenerator>
#include <QString>
#include <QVector>

#include "DataButton.h"
#include "Elevator.h"
#include "Floor.h"

Building::Building(int f, int e, int ar, int ac, QObject *parent)
    : QAbstractTableModel(parent),
      floorCount(f),
      elevatorCount(e),
      rowButtonCount(ar),
      colButtonCount(ac),
      buildingFireButton(new DataButton(true, false, false, "Building\nFIRE")),
      buildingPowerOutButton(
          new DataButton(true, false, false, "Building\nPOWER OUT")) {
    /* Initialize floors */
    for (int f_ind = 0; f_ind < floorCount; ++f_ind) {
        int floorNum = index_to_floorNum(f_ind);

        Floor *newFloor = new Floor(f_ind, floorNum, this);

        floorNum_FloorData_Map.insert(floorNum, newFloor);

        // Floor state changes imply building data changes overall
        connect(newFloor, &Floor::floorStateChanged, this,
                &Building::buildingDataChanged);
    }

    /* Initialize elevators */
    for (int e_ind = 0; e_ind < elevatorCount; ++e_ind) {
        // Generate random starting floor for each elevator
        int initFloorNum = index_to_floorNum(
            QRandomGenerator::global()->bounded(0, floorCount));

        int carId = index_to_carId(e_ind);

        Elevator *newElevator =
            new Elevator(e_ind, carId, initFloorNum, this, this);

        carId_Elevator_Map.insert(carId, newElevator);

        // Catch changes in elevator to update building data
        connect(newElevator, &Elevator::elevatorDataChanged, this,
                [newElevator, this]() {
                    emit buildingDataChanged();
                    this->updateColumn(newElevator->buildingColIndex);
                });

        connect(newElevator, &Elevator::elevatorArrived, this,
                [newElevator, this]() {
                    this->getFloor_byFloorNum(newElevator->currentFloorNum)
                        ->resetButtons();
                });

        // Catch building emergency button changes in building
        connect(buildingFireButton, &DataButton::buttonCheckedUpdate, this,
                &Building::buildingDataChanged);
        connect(buildingPowerOutButton, &DataButton::buttonCheckedUpdate, this,
                &Building::buildingDataChanged);

        // Inform elevators to compute new movement when building data changes
        connect(this, &Building::buildingDataChanged, newElevator,
                &Elevator::determineMovement);
    }
}

bool Building::buildingOnFire() const {
    return buildingFireButton->isChecked();
}
bool Building::buildingPowerOut() const {
    return buildingPowerOutButton->isChecked();
}

const QVector<int> Building::getQueuedFloors(Direction dir) const {
    QVector<int> matchingFloors;

    // Floor data map is pre-sorted.
    for (auto i = floorNum_FloorData_Map.cbegin(),
              end = floorNum_FloorData_Map.cend();
         i != end; ++i) {
        int floorNum = i.key();

        bool upMatched = (i.value()->pressedUp()) &&
                         (dir == Direction::UP || dir == Direction::NONE);
        bool downMatched = (i.value()->pressedDown()) &&
                           (dir == Direction::DOWN || dir == Direction::NONE);

        if (upMatched || downMatched) matchingFloors.append(floorNum);
    }

    // Return ascending list of all matching floor numbers.
    return matchingFloors;
}

void Building::updateColumn(int col) {
    emit dataChanged(index(0, col), index(floorCount, col));
}

int Building::rowCount(const QModelIndex & /*parent*/) const {
    return floorCount + rowButtonCount;  // Add rows used for buttons.
}

int Building::columnCount(const QModelIndex & /*parent*/) const {
    return elevatorCount + colButtonCount;  // Add columns used for buttons.
}

QVariant Building::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();

    // Only access data for elevator/floor cells, not the button cells
    if (isFloorDataIndex(row) && isElevatorIndex(col)) {
        const Elevator *elevator = getElevator_byIndex(col);

        if (index_to_floorNum(row) == elevator->currentFloorNum) {
            switch (role) {
                case Qt::DisplayRole:
                    // Key data, rendered as text
                    return elevator->getElevatorString();
                case Qt::BackgroundRole:
                    // Background brush
                    // Color the cell of current elevator location
                    switch (elevator->getDoorState()) {
                        case Elevator::DoorState::OPENING:
                            return QBrush(Qt::darkGreen);
                        case Elevator::DoorState::OPEN:
                            return QBrush(Qt::green);
                        case Elevator::DoorState::CLOSING:
                            return QBrush(Qt::darkCyan);
                        case Elevator::DoorState::CLOSED:
                        default:
                            return QBrush(Qt::cyan);
                    }
                case Qt::TextAlignmentRole:
                    /* Align text within cell.
                      Int conversion is a hack to achieve the horizontal center
                      align + vertical top align (the flags are internally
                      bitstrings), bug in Qt */
                    return int(Qt::AlignHCenter | Qt::AlignTop);
            }
        }
    }

    return QVariant();
}

QVariant Building::headerData(int section, Qt::Orientation orientation,
                              int role) const {
    if (role == Qt::DisplayRole) {
        switch (orientation) {
            case Qt::Horizontal:
                if (isElevatorIndex(section))
                    return QString("Elevator %1").arg(index_to_carId(section));
                break;
            case Qt::Vertical:
                if (isFloorDataIndex(section))
                    return QString("F%1").arg(index_to_floorNum(section));
                break;
        }
    }
    return QVariant();
}

int Building::index_to_floorNum(int index) const {
    // Floors start from 1 and increment by 1; lowest index is highest floor
    validateFloorDataIndex(index);
    return floorCount - index;  // floor number
}

int Building::index_to_carId(int index) const {
    // Car (elevator) IDs are integers incrementing from 1.
    // Lowest index is lowest car ID.
    validateElevatorIndex(index);
    return index + 1;  // car ID
}

bool Building::isFloorDataIndex(int index) const {
    return (index >= 0 && index <= floorCount - 1);
}
bool Building::isElevatorIndex(int index) const {
    return (index >= 0 && index <= elevatorCount - 1);
}
void Building::validateFloorDataIndex(int index) const {
    if (!isFloorDataIndex(index))
        throw "ERROR: Floor (row) index out of data bounds";
}
void Building::validateElevatorIndex(int index) const {
    if (!isElevatorIndex(index))
        throw "ERROR: Elevator carId (col) index out of data bounds";
}

Floor *Building::getFloor_byIndex(int index) {
    return getFloor_byFloorNum(index_to_floorNum(index));
}

Floor *Building::getFloor_byFloorNum(int floorNum) {
    if (!floorNum_FloorData_Map.contains(floorNum))
        throw "ERROR: Floor number trying to be accessed doesn't exist";
    return floorNum_FloorData_Map[floorNum];
}

Elevator *Building::getElevator_byIndex(int index) {
    return getElevator_byCarId(index_to_carId(index));
}

Elevator *Building::getElevator_byCarId(int carId) {
    if (!carId_Elevator_Map.contains(carId))
        throw "ERROR: Elevator trying to be accessed doesn't exist";
    return carId_Elevator_Map[carId];
}

const Elevator *Building::getElevator_byIndex(int index) const {
    return carId_Elevator_Map[index_to_carId(index)];
}

QVector<QWidget *> Building::getEmergencyButtons() {
    return QVector<QWidget *>{qobject_cast<QWidget *>(buildingFireButton),
                              qobject_cast<QWidget *>(buildingPowerOutButton)};
}
