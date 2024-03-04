#include "mainwindow.h"
#include "ui_mainwindow.h"

// Elevator, Floor, Passenger numbers are defined here, can be changed.
// (As per assignment scope, GUI will only accommodate 7 floors, 3 elevators, 3 passengers.)
const int MainWindow::ELEVATOR_COUNT = 7;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(released()), this, SLOT(doSomething()));

    // TODO: Init elevators and floors here
    qInfo("int = %d", ELEVATOR_COUNT);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::doSomething()
{
    qInfo("Hello world!");
}
