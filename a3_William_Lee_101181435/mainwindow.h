#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QScrollBar>
#include <QStringBuilder>
#include <QTimer>

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
    // (As per assignment scope, GUI will only accommodate 7 floors, 3
    // elevators, 3 passengers.)
    static const int ELEVATOR_COUNT = 7;
    // Length of each time chunk in the simulation, in milliseconds.
    static const int UPDATE_INTERVAL_MS = 1000;

    int timeCount;
    QTimer *updateTimer;

   private slots:
    void updateUi();
    void testFunction();
    void inlineConsoleDisplay(const QString &text);
};
#endif  // MAINWINDOW_H
