#include "mainwindow.h"

#include <QBoxLayout>
#include <QDebug>
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
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    /* Initialize building data model */
    buildingModel = new Building(FLOOR_COUNT, ELEVATOR_COUNT, 4, 1);

    /* Initialize building view */
    buildingView = ui->buildingView;

    // Make rows and columns stretch to parent
    buildingView->setModel(buildingModel);

    // Set sizes for columns
    buildingView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    buildingView->horizontalHeader()->setMinimumSectionSize(150);
    buildingView->horizontalHeader()->setMaximumSectionSize(150);

    // Set sizes for rows
    buildingView->verticalHeader()->setMinimumSectionSize(50);
    buildingView->verticalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);

    // Add buttons for each floor in the building UI.
    for (int f = 0; f < buildingModel->floorCount; ++f) {
        addIndexWidgets(f, buildingModel->elevatorCount,
                        buildingModel->getFloorButtons_byIndex(f));
    }

    // Add building-wide emergency simulation buttons.
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
        connect(el, &Elevator::textOut, this, [e, this](const QString &text) {
            this->inlineConsoleDisplay(
                QString("Elevator %1: ")
                    .arg(this->buildingModel->index_to_carId(e))
                    .append(text));
        });

        // Row to put widgets on
        int addRowIndex = buildingModel->floorCount;

        /* First row: Display panel */
        /* Floor display widget */
        QLCDNumber *floorDisplay = new QLCDNumber();

        // Set size
        QSizePolicy floorSize(QSizePolicy::Preferred, QSizePolicy::Preferred);
        floorSize.setHorizontalStretch(1);
        floorDisplay->setSizePolicy(floorSize);

        // Get max digit count needed
        floorDisplay->setDigitCount(
            QString("%1").arg(buildingModel->floorCount).length());

        /* Text display widget */
        QLabel *textDisplay = new QLabel();

        // Set size
        QSizePolicy textDispSize(QSizePolicy::Preferred,
                                 QSizePolicy::Preferred);
        textDispSize.setHorizontalStretch(3);
        textDisplay->setSizePolicy(textDispSize);
        textDisplay->setMinimumHeight(70);

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
                        QVector<QWidget *>{floorDisplay, textDisplay});

        /* Second row: open/close buttons */
        addIndexWidgets(addRowIndex++, e, el->getDoorButtonWidgets());

        /* Third row: Destination buttons, lower floors first */
        addIndexWidgets(addRowIndex++, e, el->getDestButtonWidgets());

        /* Fourth row: Simulate emergencies */
        addIndexWidgets(addRowIndex++, e, el->getEmergencyButtonWidgets());
    }
}

MainWindow::~MainWindow() {
    delete buildingModel;
    delete ui;
}

void MainWindow::addIndexWidgets(int rowIndex, int colIndex,
                                 QVector<QWidget *> widgetsToAdd,
                                 QBoxLayout::Direction layoutType) {
    QWidget *newContainer = new QWidget;  // Container widget
    QBoxLayout *newLayout;
    QVectorIterator<QWidget *> i(widgetsToAdd);

    if (widgetsToAdd.length() < 8) {
        newLayout = new QBoxLayout(layoutType, newContainer);

        while (i.hasNext()) newLayout->addWidget(i.next());
    } else {
        // Too many items, split them by multiple rows
        newLayout =
            new QBoxLayout(QBoxLayout::Direction::TopToBottom, newContainer);
        QBoxLayout *rowLayout;

        const int overflowCols = 3;
        int count = 0;

        i.toBack();
        while (i.hasPrevious()) {
            if (count % overflowCols == 0) {
                rowLayout = new QBoxLayout(QBoxLayout::Direction::LeftToRight);
                rowLayout->setSpacing(0);
                rowLayout->setContentsMargins(0, 0, 0, 0);

                newLayout->addLayout(rowLayout);
            }
            rowLayout->addWidget(i.previous());
            count++;
        }
    }

    newLayout->setSpacing(0);
    newLayout->setContentsMargins(0, 0, 0, 0);

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
