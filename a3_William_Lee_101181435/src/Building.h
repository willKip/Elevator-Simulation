#ifndef BUILDING_H
#define BUILDING_H

#include <QAbstractTableModel>
#include <QBrush>
#include <QDebug>
#include <QFont>

class Building : public QAbstractTableModel {
    Q_OBJECT
   public:
    Building(int floor_count, int elevator_count, QObject *parent = nullptr);

    // TODO: populate with floor and elevator objects

    // Implement virtual functions from Qt object
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation, int role) const override;

   private:
    int floor_count;     // Rows
    int elevator_count;  // Columns
};
#endif /* BUILDING_H */
