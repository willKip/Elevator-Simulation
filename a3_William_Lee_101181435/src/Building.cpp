#include "Building.h"

#include <QAbstractTableModel>
#include <QBrush>
#include <QDebug>
#include <QFont>
#include <QMultiHash>
#include <QPair>
#include <QRandomGenerator>
#include <QString>
#include <QTimer>
#include <QVector>
#include <QtGlobal>

#include "Elevator.h"
#include "FloorButton.h"

ElevatorData::ElevatorData(int index, int carId, int initFloorNum,
                           Building *parentBuilding, QObject *parent)
    : QObject(parent),
      index(index),
      carId(carId),
      currentFloorNum(initFloorNum),
      obj(new Elevator(carId, parentBuilding, this)),
      parentBuilding(parentBuilding),
      movementTimer(new QTimer(this)) {
    // Set up timer
    movementTimer->setInterval(movementMs);

    // Inform elevator when parent building's data changes
    connect(parentBuilding, &Building::buildingDataChanged, obj,
            &Elevator::determineMovement);

    // Initiate elevator movement when elevator informs of state change
    connect(obj, &Elevator::elevatorMovementChanged, this,
            &ElevatorData::receiveElevatorMovement);

    // TODO: handle arrival signal
    connect(obj, &Elevator::elevatorArrived, this,
            &ElevatorData::receiveElevatorMovement);

    // If the movement timer completes without interruption, move the elevator.
    connect(movementTimer, &QTimer::timeout, this, &ElevatorData::moveElevator);
}

void ElevatorData::receiveElevatorMovement() {
    switch (obj->getMovementState()) {
        case Elevator::MovementState::UPWARDS:
        case Elevator::MovementState::DOWNWARDS:
            // This slot's corresponding signal only fires when movement state
            // has changed: thus this correctly resets the timer if elevator
            // changes from e.g. moving up -> moving down.
            this->movementTimer->start();
            this->parentBuilding->updateColumn(this->index);
            break;
        case Elevator::MovementState::STOPPED:
            this->movementTimer->stop();
            this->parentBuilding->updateColumn(this->index);
            // Turn off floor's buttons since elevator arrived at floor.
            // TODO: should turn off both if elevator has no panel buttons
            // pressed todo: otherwise it is making a stop at the floor and
            // resuming, disable todo: only the button along the direction of
            // the elevator's movement.
            this->parentBuilding->getFloor_byFloorNum(this->currentFloorNum)
                ->resetButtons();
            break;
        default:
            break;
    }
}

void ElevatorData::moveElevator() {
    switch (obj->getMovementState()) {
        case Elevator::MovementState::UPWARDS:
            parentBuilding->placeElevator(carId, currentFloorNum + 1);
            break;
        case Elevator::MovementState::DOWNWARDS:
            parentBuilding->placeElevator(carId, currentFloorNum - 1);
            break;
        default:
            break;
    }
}

const QString ElevatorData::getDisplayString() const {
    return obj->getElevatorString();
}

FloorData::FloorData(int i, int fn, Building *parentBuilding, QObject *parent)
    : QObject(parent),
      index(i),
      floorNumber(fn),
      upButton(new FloorButton(floorNumber, Direction::UP, false,
                               QString("floor%1UpButton").arg(floorNumber))),
      downButton(
          new FloorButton(floorNumber, Direction::DOWN, false,
                          QString("floor%1DownButton").arg(floorNumber))),
      parentBuilding(parentBuilding) {
    // Disable buttons appropriately at the very top or bottom floor.
    if (index == 0)
        upButton->setDisabled(true);  // Top floor
    else if (index == (parentBuilding->floorCount - 1))
        downButton->setDisabled(true);  // Bottom floor

    connect(upButton, &FloorButton::buttonCheckedChanged, parentBuilding,
            &Building::buildingDataChanged);
    connect(downButton, &FloorButton::buttonCheckedChanged, parentBuilding,
            &Building::buildingDataChanged);
};

bool FloorData::pressedUp() const { return upButton->isChecked(); }
bool FloorData::pressedDown() const { return downButton->isChecked(); }
void FloorData::resetButtons() {
    upButton->setChecked(false);
    downButton->setChecked(false);
}

Building::Building(int f, int e, int ar, int ac, QObject *parent)
    : QAbstractTableModel(parent),
      floorCount(f),
      elevatorCount(e),
      rowButtonCount(ar),
      colButtonCount(ac) {
    for (int f_ind = 0; f_ind < floorCount; ++f_ind) {
        // Initialize floors (connection to UI buttons done after in MainWindow)

        int floorNum = index_to_floorNum(f_ind);

        index_floorNum_Map.insert(f_ind, floorNum);
        floorNum_FloorData_Map.insert(floorNum,
                                      new FloorData(f_ind, floorNum, this));
    }

    for (int e_ind = 0; e_ind < elevatorCount; ++e_ind) {
        // Initialize elevators

        // Generate random valid starting floor index
        int initFloorIndex = QRandomGenerator::global()->bounded(0, floorCount);
        int initFloorNum = index_to_floorNum(initFloorIndex);

        int carId = index_to_carId(e_ind);

        index_carId_Map.insert(e_ind, carId);

        carId_ElevatorData_Map.insert(
            carId, new ElevatorData(e_ind, carId, initFloorNum, this, this));
    }
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

int Building::placeElevator(int carId, int newFloorNum) {
    // This would indicate a flaw in elevator logic, it should be smart
    // enough to not attempt invalid movement.
    if (!floorNum_FloorData_Map.contains(newFloorNum))
        throw "Error: Elevator attempted invalid movement!";

    ElevatorData *elevatorData = getElevator_byCarId(carId);

    int prevFloorNum = elevatorData->currentFloorNum;

    if (prevFloorNum != newFloorNum) {
        elevatorData->currentFloorNum = newFloorNum;

        emit buildingDataChanged();

        // Inform view to update the moved elevator's column
        updateColumn(elevatorData->index);
    }

    return prevFloorNum;
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

    // Only access data for rows and columns not reserved for button placement.
    if (isFloorDataIndex(row) && isElevatorDataIndex(col)) {
        const ElevatorData *ed = getElevator_byIndex(col);

        if (index_to_floorNum(row) == ed->currentFloorNum) {
            switch (role) {
                case Qt::DisplayRole:
                    // Key data, rendered as text
                    return ed->getDisplayString();
                    break;
                case Qt::BackgroundRole:
                    // Background brush
                    // Color the cell of current elevator location
                    return QBrush(Qt::cyan);
                    break;
                case Qt::TextAlignmentRole:
                    // Align text within cell.
                    // Int conversion is a hack to achieve the horizontal center
                    // align + vertical top align (the flags are internally
                    // bitstrings).
                    return int(Qt::AlignHCenter | Qt::AlignTop);
                    break;
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
                if (isElevatorDataIndex(section))
                    return QString("Elevator %1").arg(index_carId_Map[section]);
                break;
            case Qt::Vertical:
                if (isFloorDataIndex(section))
                    return QString("F%1").arg(index_floorNum_Map[section]);
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
    validateElevatorDataIndex(index);
    return index + 1;  // car ID
}

bool Building::isFloorDataIndex(int index) const {
    return (index >= 0 && index <= floorCount - 1);
}
bool Building::isElevatorDataIndex(int index) const {
    return (index >= 0 && index <= elevatorCount - 1);
}
void Building::validateFloorDataIndex(int index) const {
    if (!isFloorDataIndex(index))
        throw "ERROR: Floor (row) index out of data bounds";
}
void Building::validateElevatorDataIndex(int index) const {
    if (!isElevatorDataIndex(index))
        throw "ERROR: Elevator carId (col) index out of data bounds";
}

FloorData *Building::getFloor_byIndex(int index) {
    validateFloorDataIndex(index);
    if (!index_floorNum_Map.contains(index))
        throw "ERROR: Nonexistent index to number mapping";
    return getFloor_byFloorNum(index_floorNum_Map[index]);
}
FloorData *Building::getFloor_byFloorNum(int floorNum) {
    if (!floorNum_FloorData_Map.contains(floorNum))
        throw "ERROR: Floor number trying to be accessed doesn't exist";
    return floorNum_FloorData_Map[floorNum];
}

ElevatorData *Building::getElevator_byIndex(int index) {
    validateElevatorDataIndex(index);
    if (!index_carId_Map.contains(index))
        throw "ERROR: Nonexistent index to carId mapping";
    return getElevator_byCarId(index_carId_Map[index]);
}
ElevatorData *Building::getElevator_byCarId(int carId) {
    if (!carId_ElevatorData_Map.contains(carId))
        throw "ERROR: Elevator trying to be accessed doesn't exist";
    return carId_ElevatorData_Map[carId];
}

const ElevatorData *Building::getElevator_byIndex(int index) const {
    validateElevatorDataIndex(index);
    if (!index_carId_Map.contains(index))
        throw "ERROR: Nonexistent index to carId mapping";
    int carId = index_carId_Map[index];

    if (!carId_ElevatorData_Map.contains(carId))
        throw "ERROR: Elevator trying to be accessed doesn't exist";
    return carId_ElevatorData_Map[carId];
}
