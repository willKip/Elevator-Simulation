#ifndef BUILDING_H
#define BUILDING_H

#include <QAbstractTableModel>
#include <QBrush>
#include <QDebug>
#include <QFont>
#include <QMultiHash>
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
    enum class EmergencyState { FIRE, POWER_OUT };

    Building(int floor_count, int elevator_count, int add_rows = 0,
             int add_cols = 0, QObject *parent = nullptr);

    // Implement virtual functions from QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation, int role) const override;

    // Additional non-data rows and columns can be allocated for buttons.
    // todo: move back
    const int floor_count;     // Rows
    const int elevator_count;  // Columns
    const int add_rows;
    const int add_cols;
    std::vector<std::vector<Elevator *>> buildingTable;

    /**
     * Helpers for accessing the 0-indexed table with floor numbers and car IDs.
     */
    // Return floor number from index. Do the inverse if the flag is set.
    int index_to_floorNum(int x, bool inverse = false) const;
    // Return car ID from index. Do the inverse if the flag is set.
    int index_to_carId(int x, bool inverse = false) const;

   private slots:
    // Move elevator one floor in the specified direction.
    void moveElevator(Elevator::Direction);

   private:
    /**
     * 2D array (matrix) representing the building's elevator positions.
     * Floors are rows, elevators are columns. Matrix stores pointers to
     * elevators.
     */
    // std::vector<std::vector<Elevator *>> buildingTable;

    /**
     * Map of floor numbers to currently pressed buttons.
     * A floor can have multiple buttons pressed (up/down).
     */
    QMultiHash<int, std::vector<Elevator::Direction>> floorRequests;

    /**
     * Set the elevator of the given ID to the given floor.
     * Returns the previous floor number.
     */
    int placeElevator(int carId, int newFloorNum);

    // Return the floor of the elevator with the given ID.
    int getElevatorFloor(int carId) const;
};
#endif /* BUILDING_H */
