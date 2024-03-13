#include "mainwindow.h"

#include <QBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QString>
#include <QVector>
#include <QVectorIterator>
#include <QWidget>

#include "Building.h"
#include "Elevator.h"
#include "Floor.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Initialize building data model.
    buildingModel = new Building(FLOOR_COUNT, ELEVATOR_COUNT, 3, 1);

    /**
     * Initialize building view in UI.
     */
    buildingView = ui->buildingView;

    // Make rows and columns stretch to parent
    buildingView->setModel(buildingModel);

    // Make vertical and horizontal headers (elevator, floor number labels)
    // fill entire available space.
    buildingView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    buildingView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Add buttons for each floor in the building UI.
    for (int f = 0; f < buildingModel->floorCount; ++f) {
        Floor *fd = buildingModel->getFloor_byIndex(f);

        addButtons(f, buildingModel->elevatorCount, fd->getButtonWidgets(),
                   false);
    }

    // Add panel and display buttons for each elevator.
    for (int e = 0; e < buildingModel->elevatorCount; ++e) {
        Elevator *el = buildingModel->getElevator_byIndex(e);

        // First row: open/close buttons
        addButtons(buildingModel->floorCount, e, el->getDoorButtonWidgets(),
                   false);

        // Second row: Destination buttons, lower floors first
        addButtons(buildingModel->floorCount + 1, e, el->getDestButtonWidgets(),
                   false);
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::addButtons(int rowIndex, int colIndex,
                            QVector<QWidget *> buttonsToAdd,
                            bool layoutIsVertical) {
    // Container widget
    QWidget *newContainer = new QWidget;

    // Set horizontal or vertical layout on the container
    QBoxLayout *newLayout = new QBoxLayout(
        (layoutIsVertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight),
        newContainer);
    newLayout->setSpacing(0);
    newLayout->setContentsMargins(0, 0, 0, 0);

    QVectorIterator<QWidget *> i(buttonsToAdd);
    while (i.hasNext()) newLayout->addWidget(i.next());

    ui->buildingView->setIndexWidget(buildingModel->index(rowIndex, colIndex),
                                     newContainer);
}

void MainWindow::inlineConsoleDisplay(const QString &text) {
    QLabel *prevTextLabel = ui->textOutput;
    prevTextLabel->setText(QString("%1\n%2").arg(prevTextLabel->text(), text));

    // Scroll to the bottom of the inline terminal.
    QScrollBar *sb = ui->outputScroll->verticalScrollBar();
    sb->setValue(sb->maximum());
}
