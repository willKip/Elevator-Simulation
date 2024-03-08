#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScrollBar>
#include <QStringBuilder>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>

#include "Building.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private:
    Ui::MainWindow *ui;

    // PROGRAM CONSTANTS
    // Elevator, Floor, Passenger numbers are defined here, can be changed.
    // (As per assignment scope, GUI is only designed to accommodate 7 floors
    // and 3 elevators).)
    static const int FLOOR_COUNT = 7;
    static const int ELEVATOR_COUNT = 3;
    // Length of each time chunk in the simulation, in milliseconds.
    static const int UPDATE_INTERVAL_MS = 1000;

    int timeCount;
    QTimer *updateTimer;

    bool buttonPressed;  // TODO: temp

    Building *buildingModel;
    QTableView *buildingView;

   signals:
    void testSig(Elevator::Direction);  // todo: temp

   public slots:
    void testSlot();        // todo: temp
    void doSomething(int);  // todo: temp

   private slots:
    void updateUi();
    void testFunction();
    void inlineConsoleDisplay(const QString &text);
};
#endif  // MAINWINDOW_H
