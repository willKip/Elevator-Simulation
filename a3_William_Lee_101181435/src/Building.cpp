#include "Building.h"

Building::Building(int f, int e, QObject *parent)
    : QAbstractTableModel(parent) {
    floor_count = f;
    elevator_count = e;
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