#include "Building.h"

#include <QAbstractTableModel>

Building::Building(int f, int e, QObject *parent)
    : floor_count(f), elevator_count(e), QAbstractTableModel(parent) {
    // Initialize matrix
    buildingTable = new Elevator **[floor_count];
    for (int i = 0; i < elevator_count; i++) {
        buildingTable[i] = new Elevator *[floor_count];
    }

    for (int i = 0; i < elevator_count; i++) {
        // TODO: initial floor randomize?
        buildingTable[1][i] = new Elevator(index_to_carId(i), floor_count);
    }
}

int Building::rowCount(const QModelIndex & /*parent*/) const {
    return floor_count;
}

int Building::columnCount(const QModelIndex & /*parent*/) const {
    return elevator_count;
}

QVariant Building::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();
    // generate a log message when this method gets called
    qDebug() << QString("row %1, col%2, role %3").arg(row).arg(col).arg(role);

    switch (role) {
        case Qt::DisplayRole:
            return QString("Row%1,\nColumn%2").arg(row + 1).arg(col + 1);
        // Color the cell of current elevator location
        case Qt::BackgroundRole:
            return QBrush(Qt::cyan);
            break;
    }
    return QVariant();
}

QVariant Building::headerData(int section, Qt::Orientation orientation,
                              int role) const {
    if (role == Qt::DisplayRole) {
        // TODO: make dynamic based on its elements.
        switch (orientation) {
            case Qt::Horizontal:
                return QString("Elevator %1").arg(section + 1);

            case Qt::Vertical:
                return QString("F%1").arg(floor_count - section);
                break;
        }
    }
    return QVariant();
}

int Building::index_to_floorNum(int x, bool inverse) {
    // Floors start from 1 and increment by 1.
    if (inverse) {
        // Floor number to table row index
        if (x > floor_count || x < 1) {
            throw "ERROR: invalid floor number";
        }
        return x - floor_count;  // index
    } else {
        // Row index to floor number
        if (x + 1 > floor_count || x < 0) {
            throw "ERROR: building table floor (row) index out of bounds";
        }
        return floor_count - x;  // floor number
    }
}

int Building::index_to_carId(int x, bool inverse) {
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