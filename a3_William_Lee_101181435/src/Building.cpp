#include "Building.h"

#include <QAbstractTableModel>
#include <QMultiHash>
#include <QRandomGenerator>
#include <QVector>
#include <QtGlobal>

#include "Direction.h"
#include "FloorButton.h"

Building::Building(int f, int e, int ar, int ac, QObject *parent)
    : QAbstractTableModel(parent),
      floor_count(f),
      elevator_count(e),
      add_rows(ar),
      add_cols(ac),
      buildingTable(building_table_t(
          floor_count, building_column_t(elevator_count, nullptr))) {
    /**
     * Initialize building table
     */
    Elevator *newElevator;
    int initFloorNum;
    for (int elevatorIndex = 0; elevatorIndex < elevator_count;
         elevatorIndex++) {
        // Initialize each elevator on a random floor.
        initFloorNum = index_to_floorNum(
            QRandomGenerator::global()->bounded(0, floor_count));

        newElevator = new Elevator(index_to_carId(elevatorIndex), initFloorNum);

        buildingTable[index_to_floorNum(initFloorNum, true)][elevatorIndex] =
            newElevator;

        connect(newElevator, &Elevator::elevatorMoving, this,
                &Building::moveElevator);
    }
}

void Building::moveElevator(Direction direction) {
    Elevator *elevator = qobject_cast<Elevator *>(sender());

    int carId = elevator->getCarId();
    int currentFloor = getElevatorFloor(carId);
    int newFloor;

    switch (direction) {
        case Direction::UP:
            newFloor = currentFloor + 1;
            break;
        case Direction::DOWN:
            newFloor = currentFloor - 1;
            break;
    }

    try {
        placeElevator(carId, newFloor);
    } catch (...) {
        // This would indicate a flaw in elevator logic, it should be smart
        // enough to not attempt invalid movement.
        throw "Error: Elevator attempted invalid movement!";
    }
}

int Building::placeElevator(int carId, int newFloorNum) {
    int prevFloorNum = getElevatorFloor(carId);
    int prevFloorRow = index_to_floorNum(prevFloorNum, true);
    int elevatorColumn = index_to_carId(carId, true);

    // Null the previous location's pointer and move the elevator's pointer to
    // the new floor.
    Elevator *e = buildingTable[prevFloorRow][elevatorColumn];
    buildingTable[prevFloorRow][elevatorColumn] = nullptr;
    buildingTable[index_to_floorNum(newFloorNum, true)][elevatorColumn] = e;

    e->setCurrentFloor(newFloorNum);

    // Inform view to update the moved elevator's column
    emit dataChanged(index(0, elevatorColumn),
                     index(floor_count, elevatorColumn));

    return prevFloorNum;
}

int Building::getElevatorFloor(int carId) const {
    int elevatorColumn = index_to_carId(carId, true);

    Elevator *currElevator;
    for (int f_index = 0; f_index < floor_count; f_index++) {
        currElevator = buildingTable[f_index][elevatorColumn];

        if (currElevator) return index_to_floorNum(f_index);
    }

    // No elevator was found after iterating over all possible floors!
    throw "Error: Elevator missing from its shaft?";
}

int Building::rowCount(const QModelIndex & /*parent*/) const {
    return floor_count + add_rows;  // Add rows used for buttons.
}

int Building::columnCount(const QModelIndex & /*parent*/) const {
    return elevator_count + add_cols;  // Add columns used for buttons.
}

QVariant Building::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();

    // Only access data for non-button columns and rows.
    if (row < floor_count && col < elevator_count) {
        Elevator *e = buildingTable[row][col];

        switch (role) {
            case Qt::DisplayRole:
                // Key data, rendered as text
                if (e) {
                    return e->getElevatorString();
                } else {
                    return QString("");
                }
                break;
            case Qt::BackgroundRole:
                // Background brush
                if (e) {
                    // Color the cell of current elevator location
                    return QBrush(Qt::cyan);
                }
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

    return QVariant();
}

void Building::updateFloorRequests() {
    // Slot: When floor button is pressed, toggle its corresponding entry in the
    // floor requests data structure and update the button.
    FloorButton *fb = qobject_cast<FloorButton *>(sender());
    Direction fb_dir = fb->getDirection();
    int fb_floorNum = fb->getFloorNum();

    bool inFloorRequests = floorRequests.contains(fb_dir, fb_floorNum);

    if (inFloorRequests) {
        floorRequests.remove(fb_dir, fb_floorNum);
    } else {
        floorRequests.insert(fb_dir, fb_floorNum);
    }

    fb->setChecked(!inFloorRequests);
}

QVariant Building::headerData(int section, Qt::Orientation orientation,
                              int role) const {
    if (role == Qt::DisplayRole) {
        switch (orientation) {
            case Qt::Horizontal:
                if (section < elevator_count)
                    return QString("Elevator %1").arg(index_to_carId(section));
                break;
            case Qt::Vertical:
                if (section < floor_count)
                    return QString("F%1").arg(index_to_floorNum(section));
                break;
        }
    }
    return QVariant();
}

int Building::index_to_floorNum(int x, bool inverse) const {
    // Floors start from 1 and increment by 1.
    if (inverse) {
        // Floor number to table row index
        if (x > floor_count || x < 1) {
            throw "ERROR: invalid floor number";
        }
        return floor_count - x;  // index
    } else {
        // Row index to floor number
        if (x + 1 > floor_count || x < 0) {
            throw "ERROR: building table floor (row) index out of bounds";
        }
        return floor_count - x;  // floor number
    }
}

int Building::index_to_carId(int x, bool inverse) const {
    // Car (elevator) IDs are incremental integers starting from 1.
    if (inverse) {
        // Car ID to table column index
        if (x > elevator_count || x < 1) {
            throw "ERROR: invalid car id";
        }
        return x - 1;  // index
    } else {
        // Column index to car ID
        if (x + 1 > elevator_count || x < 0) {
            throw "ERROR: building table carId (col) index out of bounds";
        }
        return x + 1;  // floor number
    }
}
