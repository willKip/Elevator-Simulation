#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QVector>
#include <QWidget>

class Building;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// TODO: cleanup
class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private:
    Ui::MainWindow *ui;

    // PROGRAM CONSTANTS
    // Elevator and Floor numbers are defined here. Can be changed and the GUI
    // will reflect it dynamically, too high values are not advised.
    static const int FLOOR_COUNT = 7;
    static const int ELEVATOR_COUNT = 3;

    Building *buildingModel;
    QTableView *buildingView;

    // Add fully aligned and distributed widgets to specified index of building
    // View. Horizontal layout unless specified otherwise in the boolean.
    void addIndexWidgets(int rowIndex, int colIndex,
                         QVector<QWidget *> buttonsToAdd,
                         bool layoutIsVertical = false);

   private slots:
    // Display specified text to the inline console.
    void inlineConsoleDisplay(const QString &text);
};
#endif  // MAINWINDOW_H
