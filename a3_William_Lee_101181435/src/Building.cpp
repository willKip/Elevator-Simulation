#include "Building.h"

#include <QAbstractTableModel>
#include <QBrush>
#include <QDebug>
#include <QFont>
#include <QMultiHash>
#include <QMutex>
#include <QPair>
#include <QRandomGenerator>
#include <QString>
#include <QTimer>
#include <QVector>
#include <QtGlobal>

#include "Elevator.h"
#include "FloorButton.h"

Building::Building(int f, int e, int ar, int ac, QObject *parent)
    : QAbstractTableModel(parent),
      floorCount(f),
      elevatorCount(e),
      buttonRowCount(ar),
      buttonColCount(ac) {
    // Initialize floors (connection to UI buttons done after in MainWindow)
    for (int f_ind = 0; f_ind < floorCount; ++f_ind) {
        int floorNum = index_to_floorNum(f_ind);

        floorNum_toIndexMap.insert(floorNum, f_ind);

        indexFloorMap.insert(f_ind, new FloorData(f_ind, floorNum));
    }

    // Initialize elevators
    for (int e_ind = 0; e_ind < elevatorCount; ++e_ind) {
        // Generate random valid starting floor index
        int initFloorIndex = QRandomGenerator::global()->bounded(0, floorCount);
        int initFloorNum = index_to_floorNum(initFloorIndex);
        int carId = index_to_carId(e_ind);

        carId_toIndexMap.insert(carId, e_ind);

        indexElevatorMap.insert(
            e_ind, new ElevatorData(e_ind, carId, initFloorNum, this));
    }

    // // TODO: Move declarations to header
    // QTimer *moveTimer = new QTimer(this);

    // moveTimer->connect(moveTimer, &QTimer::timeout, this, &MainWindow::Test);
    // moveTimer->start(2000);
}

// TODO: Instantiate elevator loop for each

void Building::moveElevator(int carId,
                            Elevator::MovementState elevatorDirection) {
    int currentFloor = getElevator_byCarId(carId)->currentFloorNum;
    int newFloor;

    switch (elevatorDirection) {
        case Elevator::MovementState::UPWARDS:
            newFloor = currentFloor + 1;
            break;
        case Elevator::MovementState::DOWNWARDS:
            newFloor = currentFloor - 1;
            break;
        case Elevator::MovementState::STOPPED:
            newFloor = currentFloor;
            break;
    }

    // Delay before elevator completes movement

    try {
        placeElevator(carId, newFloor);
    } catch (...) {
        // This would indicate a flaw in elevator logic, it should be smart
        // enough to not attempt invalid movement.
        throw "Error: Elevator attempted invalid movement!";
    }
}

int Building::placeElevator(int carId, int newFloorNum) {
    ElevatorData *elevatorData = getElevator_byCarId(carId);

    int prevFloorNum = elevatorData->currentFloorNum;

    if (prevFloorNum != newFloorNum) {
        elevatorData->currentFloorNum = newFloorNum;

        // Inform view to update the moved elevator's column
        emit dataChanged(index(0, elevatorData->index),
                         index(floorCount, elevatorData->index));
    }

    return prevFloorNum;
}

int Building::rowCount(const QModelIndex & /*parent*/) const {
    return floorCount + buttonRowCount;  // Add rows used for buttons.
}

int Building::columnCount(const QModelIndex & /*parent*/) const {
    return elevatorCount + buttonColCount;  // Add columns used for buttons.
}

QVariant Building::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();

    // Only access data for rows and columns not reserved for button placement.
    if (row < floorCount && col < elevatorCount) {
        const ElevatorData *ed = getElevator_byIndex(col);

        if (index_to_floorNum(row) == ed->currentFloorNum) {
            switch (role) {
                case Qt::DisplayRole:
                    // Key data, rendered as text
                    return ed->obj->getElevatorString();
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

void Building::updateFloorRequests() {
    // Slot: When floor button is pressed, toggle its corresponding entry in the
    // floor requests data structure and update the button.

    // Prevent user interaction and elevator arrival from conflicting
    mutex.lock();

    FloorButton *fb = qobject_cast<FloorButton *>(sender());
    fb->flipChecked();
    emit dataChanged(index(0, elevatorCount), index(floorCount, elevatorCount));

    mutex.unlock();
}

QVariant Building::headerData(int section, Qt::Orientation orientation,
                              int role) const {
    if (role == Qt::DisplayRole) {
        switch (orientation) {
            case Qt::Horizontal:
                if (section < elevatorCount)
                    return QString("Elevator %1").arg(index_to_carId(section));
                break;
            case Qt::Vertical:
                if (section < floorCount)
                    return QString("F%1").arg(index_to_floorNum(section));
                break;
        }
    }
    return QVariant();
}

int Building::index_to_floorNum(int index) const {
    // Floors start from 1 and increment by 1; lowest index is highest floor
    if (index + 1 > floorCount || index < 0)
        throw "ERROR: building table floor (row) index out of bounds";
    return floorCount - index;  // floor number
}

int Building::index_to_carId(int index) const {
    // Car (elevator) IDs are integers incrementing from 1.
    // Lowest index is lowest car ID.
    if (index + 1 > elevatorCount || index < 0) {
        throw "ERROR: building table carId (col) index out of bounds";
    }
    return index + 1;  // car ID
}

Building::FloorData *Building::getFloor_byIndex(int index) {
    return indexFloorMap[index];
}
Building::FloorData *Building::getFloor_byFloorNum(int floorNum) {
    return indexFloorMap[floorNum_toIndexMap[floorNum]];
}
Building::ElevatorData *Building::getElevator_byIndex(int index) {
    return indexElevatorMap[index];
}
Building::ElevatorData *Building::getElevator_byCarId(int carId) {
    return indexElevatorMap[carId_toIndexMap[carId]];
}

const Building::ElevatorData *Building::getElevator_byIndex(int index) const {
    return indexElevatorMap[index];
}
