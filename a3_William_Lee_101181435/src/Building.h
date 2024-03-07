#ifndef BUILDING_H
#define BUILDING_H

#include <QAbstractTableModel>
#include <QBrush>
#include <QDebug>
#include <QFont>
#include <vector>

#include "Elevator.h"

/**
 * Class simulating a building with elevators.
 * Extends QAbstractTableModel, serving the role of Model in the MVC paradigm.
 * Member variables:
 * - floor_count        Number of floors in the building.
 * - elevator_count     Number of elevators in the building.
 * - buildingTable      2-dimensional matrix with rows as floors, columns as
 *                      elevators.
 * TODO
 */
class Building : public QAbstractTableModel {
    Q_OBJECT
   public:
    Building(int floor_count, int elevator_count, QObject *parent = nullptr);

    // Implement virtual functions from QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation, int role) const override;

   private:
    int floor_count;     // Rows
    int elevator_count;  // Columns
    /**
     * 2D array (matrix) representing the building's elevator positions.
     * Floors are rows, elevators are columns. Matrix stores pointers to
     * elevators.
     */
    Elevator ***buildingTable;

    /**
     * Helpers for accessing the 0-indexed table with floor numbers and car IDs.
     */
    // Return floor number from index. Do the inverse if the flag is set.
    int index_to_floorNum(int x, bool inverse = false);
    // Return car ID from index. Do the inverse if the flag is set.
    int index_to_carId(int x, bool inverse = false);
};
#endif /* BUILDING_H */
