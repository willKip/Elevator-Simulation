#include "mainwindow.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QMap>
#include <QPushButton>
#include <QSignalMapper>
#include <QStandardItem>
#include <QString>
#include <QVBoxLayout>

#include "Building.h"
#include "FloorButton.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->testButton, SIGNAL(released()), this, SLOT(testFunction()));

    // Initialize number of iterations since simulation started.
    timeCount = 0;

    // Initialize building data model.
    buildingModel = new Building(FLOOR_COUNT, ELEVATOR_COUNT, 0, 1);

    /**
     * Initialize building view in UI.
     */
    buildingView = ui->buildingView;
    // Make rows and columns stretch to parent
    buildingView->setModel(buildingModel);
    buildingView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    buildingView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    /**
     * Initialize and add buttons for each floor in the building UI.
     */
    QWidget *floorButtonContainer;
    QVBoxLayout *floorButtonLayout;
    FloorButton *up, *down;
    int floorNum;

    for (int f = 0; f < FLOOR_COUNT; f++) {
        floorNum = buildingModel->index_to_floorNum(f);

        // Container widget
        floorButtonContainer = new QWidget;

        // Set layout on the container
        floorButtonLayout = new QVBoxLayout(floorButtonContainer);
        floorButtonLayout->setSpacing(0);
        floorButtonLayout->setContentsMargins(0, 0, 0, 0);

        // Initialize buttons
        up = new FloorButton(floorNum, Direction::UP, false,
                             QString("floor%1UpButton").arg(floorNum));
        down = new FloorButton(floorNum, Direction::DOWN, false,
                               QString("floor%1DownButton").arg(floorNum));

        // Disable non-applicable floor buttons
        if (f == 0) {
            // Top floor
            up->setDisabled(true);
        } else if (f == FLOOR_COUNT - 1) {
            // Bottom floor
            down->setDisabled(true);
        }

        connect(up, &FloorButton::pressed, buildingModel,
                &Building::updateFloorRequests);
        connect(down, &FloorButton::pressed, buildingModel,
                &Building::updateFloorRequests);

        floorButtonLayout->addWidget(up);
        floorButtonLayout->addWidget(down);

        ui->buildingView->setIndexWidget(
            buildingModel->index(f, ELEVATOR_COUNT), floorButtonContainer);
    }

    buttonPressed = false;

    // Update view
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateUi()));
    updateTimer->start(UPDATE_INTERVAL_MS);
}

MainWindow::~MainWindow() {
    delete ui;
    delete updateTimer;
}

void MainWindow::updateUi() {
    inlineConsoleDisplay(QString("Iteration [%1]").arg(timeCount));

    if (buttonPressed) {
        inlineConsoleDisplay("Button was pressed this iteration.");
        buttonPressed = false;
    } else {
        inlineConsoleDisplay("NOT pressed.");
    }

    timeCount += 1;  // TODO: shouldnt be in the "ui update function"
}

void MainWindow::testFunction() {
    buttonPressed = true;
    inlineConsoleDisplay("Button pressed!");
}

// Display specified text to the inline console.
void MainWindow::inlineConsoleDisplay(const QString &text) {
    // TODO: Display time before each output.
    QLabel *prevTextLabel = ui->textOutput;
    prevTextLabel->setText(QString(prevTextLabel->text() % "\n" % text));

    // Scroll to the bottom of the inline terminal.
    QScrollBar *sb = ui->outputScroll->verticalScrollBar();
    sb->setValue(sb->maximum());
}
