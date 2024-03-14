#include "mainwindow.h"

#include <QBoxLayout>
#include <QLCDNumber>
#include <QLabel>
#include <QScrollBar>
#include <QSizePolicy>
#include <QString>
#include <QVector>
#include <QVectorIterator>
#include <QWidget>

#include "Building.h"
#include "Elevator.h"
#include "Floor.h"
#include "ui_mainwindow.h"

// TODO: cleanup
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Initialize building data model.
    buildingModel = new Building(FLOOR_COUNT, ELEVATOR_COUNT, 4, 1);

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

        addIndexWidgets(f, buildingModel->elevatorCount, fd->getButtonWidgets(),
                        false);
    }

    /**
     * Add building-wide emergency simulation buttons.
     */
    QHBoxLayout *buildingButtonLayout = ui->buildingButtonLayout;
    buildingButtonLayout->setSpacing(0);
    buildingButtonLayout->setContentsMargins(0, 0, 0, 0);

    QVector<QWidget *> buildingEmergencyButtons =
        buildingModel->getEmergencyButtons();

    QVectorIterator<QWidget *> i(buildingEmergencyButtons);
    while (i.hasNext()) {
        buildingButtonLayout->addWidget(i.next());
    }

    /**
     * Add panel and display buttons for each elevator, connect to text output,
     * add buttons to simulate emergency situations.
     */
    for (int e = 0; e < buildingModel->elevatorCount; ++e) {
        Elevator *el = buildingModel->getElevator_byIndex(e);

        // Receive text output signals for displaying in inline console
        connect(el, &Elevator::textOut, this, [el, this](const QString &text) {
            this->inlineConsoleDisplay(
                QString("Elevator %1: ").arg(el->carId).append(text));
        });

        // Row to put widgets on
        int addRowIndex = buildingModel->floorCount;

        /**
         * First row: Display panel
         */
        /* Floor display widget */
        QLCDNumber *floorDisplay = new QLCDNumber();

        // Set relative size
        QSizePolicy floorSize(QSizePolicy::Preferred, QSizePolicy::Preferred);
        floorSize.setHorizontalStretch(1);
        floorDisplay->setSizePolicy(floorSize);

        // Get max digit count needed
        floorDisplay->setDigitCount(
            QString("%1").arg(buildingModel->floorCount).length());

        /* Text display widget */
        QLabel *textDisplay = new QLabel();

        // Set relative size
        QSizePolicy textDispSize(QSizePolicy::Preferred,
                                 QSizePolicy::Preferred);
        textDispSize.setHorizontalStretch(3);
        textDisplay->setSizePolicy(textDispSize);

        // Make text wrap around
        textDisplay->setWordWrap(true);

        // Initial values
        floorDisplay->display(el->currentFloorNum);
        textDisplay->setText(el->getTextDisplay());

        // Update displays when there is a change to elevator data
        connect(el, &Elevator::elevatorDataChanged, floorDisplay,
                [el, floorDisplay, textDisplay]() {
                    floorDisplay->display(el->currentFloorNum);
                    textDisplay->setText(el->getTextDisplay());
                });

        addIndexWidgets(addRowIndex++, e,
                        QVector<QWidget *>{floorDisplay, textDisplay}, false);

        /* Second row: open/close buttons */
        addIndexWidgets(addRowIndex++, e, el->getDoorButtonWidgets(), false);

        /* Third row: Destination buttons, lower floors first */
        addIndexWidgets(addRowIndex++, e, el->getDestButtonWidgets(), false);

        /* Fourth row: Simulate emergencies */
        addIndexWidgets(addRowIndex++, e, el->getEmergencyButtonWidgets(),
                        false);
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::addIndexWidgets(int rowIndex, int colIndex,
                                 QVector<QWidget *> widgetsToAdd,
                                 bool layoutIsVertical) {
    // Container widget
    QWidget *newContainer = new QWidget;

    // Set horizontal or vertical layout on the container
    QBoxLayout *newLayout = new QBoxLayout(
        (layoutIsVertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight),
        newContainer);
    newLayout->setSpacing(0);
    newLayout->setContentsMargins(0, 0, 0, 0);

    QVectorIterator<QWidget *> i(widgetsToAdd);
    while (i.hasNext()) {
        newLayout->addWidget(i.next());
    }

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
