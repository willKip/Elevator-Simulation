#include "mainwindow.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QMap>
#include <QPushButton>
#include <QSignalMapper>
#include <QStandardItem>
#include <QString>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

#include "Building.h"
#include "Elevator.h"
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
    buildingView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    buildingView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Add buttons for each floor in the building UI.
    for (int f = 0; f < buildingModel->floorCount; ++f) {
        FloorData *fd = buildingModel->getFloor_byIndex(f);

        addButtons(f, buildingModel->elevatorCount,
                   QVector<QWidget *>{fd->upButton, fd->downButton}, false);
    }

    // Add panel and display buttons for each elevator.
    for (int e = 0; e < buildingModel->elevatorCount; ++e) {
        Elevator *el = buildingModel->getElevator_byIndex(e);

        // First row: open/close buttons
        addButtons(buildingModel->floorCount, e,
                   QVector<QWidget *>{el->openButton, el->closeButton}, false);

        // Second row: Destination buttons, lower floors first
        QVector<QWidget *> floorPanel;
        for (int f = 0; f < buildingModel->floorCount; ++f)
            floorPanel.prepend(
                el->destinationButtons[buildingModel->index_to_floorNum(f)]);
        addButtons(buildingModel->floorCount + 1, e, floorPanel, false);
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
    prevTextLabel->setText(QString(prevTextLabel->text() % "\n" % text));

    // Scroll to the bottom of the inline terminal.
    QScrollBar *sb = ui->outputScroll->verticalScrollBar();
    sb->setValue(sb->maximum());
}
