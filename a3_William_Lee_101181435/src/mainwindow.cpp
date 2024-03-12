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
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // TODO: remove
    connect(ui->testButton, SIGNAL(released()), this, SLOT(testFunction()));

    // Initialize building data model.
    buildingModel = new Building(FLOOR_COUNT, ELEVATOR_COUNT, 3, 1);

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
    for (int f = 0; f < FLOOR_COUNT; ++f) {
        // Container widget
        QWidget *floorButtonContainer = new QWidget;

        // Set layout on the container
        QHBoxLayout *floorButtonLayout = new QHBoxLayout(floorButtonContainer);
        floorButtonLayout->setSpacing(0);
        floorButtonLayout->setContentsMargins(0, 0, 0, 0);

        FloorData *fd = buildingModel->getFloor_byIndex(f);

        floorButtonLayout->addWidget(fd->upButton);
        floorButtonLayout->addWidget(fd->downButton);

        ui->buildingView->setIndexWidget(
            buildingModel->index(f, ELEVATOR_COUNT), floorButtonContainer);
    }

    buttonPressed = false;
}

MainWindow::~MainWindow() {
    delete ui;
    delete updateTimer;
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
