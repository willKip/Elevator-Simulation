#include "mainwindow.h"
#include "ui_mainwindow.h"

// Elevator, Floor, Passenger numbers are defined here, can be changed.
// (As per assignment scope, GUI will only accommodate 7 floors, 3 elevators, 3 passengers.)
const int MainWindow::ELEVATOR_COUNT = 7;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->testButton, SIGNAL(released()), this, SLOT(testFunction()));

    // TODO: Init elevators and floors here
    qInfo("int = " + ELEVATOR_COUNT);

    // TODO: Update view (get updates from each elevator)
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testFunction()
{
    inlineConsoleDisplay("Hello world!");
}

// Display specified text to the inline console.
void MainWindow::inlineConsoleDisplay(const QString &text)
{
    // TODO: Display time before each output.
    QLabel *prevTextLabel = ui->textOutput;
    prevTextLabel->setText(QString(prevTextLabel->text() % "\n" % text));

    // Scroll to the bottom of the inline terminal.
    QScrollBar *sb = ui->outputScroll->verticalScrollBar();
    sb->setValue(sb->maximum());
}
