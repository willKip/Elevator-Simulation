#ifndef FLOOR_H
#define FLOOR_H

#include <QObject>
#include <QVector>
#include <QWidget>

#include "FloorButton.h"

class Building;

// Helper data class for better management of associated data
// TODO: documentation
// TODO: cleanup
class Floor : public QObject {
    Q_OBJECT
   public:
    Floor(int index, int floorNumber, Building *parentBuilding,
          QObject *parent = nullptr);

    const int index;        // Data index within a Building
    const int floorNumber;  // Unique floor number within a Building

    // Return checked state of floor buttons
    bool pressedUp() const;
    bool pressedDown() const;

    QVector<QWidget *> getButtonWidgets();

    // Set all buttons of the floor to unchecked
    void resetButtons();

   signals:
    void floorStateChanged();

   private:
    Building *const parentBuilding;  // Pointer to Building Floor belongs to

    // Directional buttons of the floor
    FloorButton *const upButton;
    FloorButton *const downButton;
};

#endif /* FLOOR_H */
