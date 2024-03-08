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

    // Style buttons in building view
    buildingView->setStyleSheet(
        "QPushButton::checked {"
        "background-color: rgba(225, 0, 0, 60%);"
        "}");

    /**
     * Initialize and add buttons for each floor in the building UI.
     */
    QWidget *floorButtonContainer;
    QVBoxLayout *floorButtonLayout;
    QPushButton *up, *down;
    for (int f = 0; f < FLOOR_COUNT; f++) {
        // Container widget
        floorButtonContainer = new QWidget;

        // Layout set on the container
        floorButtonLayout = new QVBoxLayout(floorButtonContainer);
        floorButtonLayout->setSpacing(0);
        floorButtonLayout->setContentsMargins(0, 0, 0, 0);

        // Initialize up button
        up = new QPushButton(this);
        up->setText("UP ▲");
        up->setCheckable(true);
        up->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        up->setObjectName(QString("floor%1UpButton")
                              .arg(buildingModel->index_to_floorNum(f)));

        // Initialize down button
        down = new QPushButton(this);
        down->setText("DOWN ▼");
        down->setCheckable(true);
        down->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        down->setObjectName(QString("floor%1DownButton")
                                .arg(buildingModel->index_to_floorNum(f)));

        // Disable non-applicable floor buttons
        if (f == 0) {
            // Top floor
            up->setDisabled(true);
        } else if (f == FLOOR_COUNT - 1) {
            // Bottom floor
            down->setDisabled(true);
        }

        floorButtonLayout->addWidget(up);
        floorButtonLayout->addWidget(down);

        ui->buildingView->setIndexWidget(
            buildingModel->index(f, ELEVATOR_COUNT), floorButtonContainer);

        // signalMapper->setMapping(cartButton, i);
        // connect(cartButton, SIGNAL(clicked(bool)), signalMapper,
        // SLOT(map()));
    }

    // QPushButton *up = new QPushButton(this);
    // up->setCheckable(true);
    // up->setText("UP");
    // up->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // QPushButton *down = new QPushButton(this);
    // down->setCheckable(true);
    // down->setText("DOWN");
    // down->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // floorButtonLayout->addWidget(up);
    // floorButtonLayout->addWidget(down);
    // ui->buildingView->setIndexWidget(buildingModel->index(0, 3),
    //                                  floorButtonContainer);

    // QSignalMapper *signalMapper = new QSignalMapper(this);
    // QWidget *floorButtonContainer;
    // QVBoxLayout *floorButtonLayout;
    // QCheckBox *up, *down;
    // for (int i = 0; i < ELEVATOR_COUNT; i++) {
    //     floorButtonContainer = new QWidget;
    //     floorButtonLayout = new QVBoxLayout(floorButtonContainer);

    //     auto item = buildingModel->index(i, ELEVATOR_COUNT + 1);
    //     up = new QCheckBox("Up");
    //     down = new QCheckBox("Down");
    //     up->setObjectName(QString("floor%1UpButton").arg(i + 1));
    //     down->setObjectName(QString("floor%1DownButton").arg(i + 1));

    //     floorButtonLayout->addWidget(up);
    //     floorButtonLayout->addWidget(down);

    //     ui->buildingView->setIndexWidget(item, floorButtonContainer);

    //     // signalMapper->setMapping(cartButton, i);
    //     // connect(cartButton, SIGNAL(clicked(bool)), signalMapper,
    //     // SLOT(map()));
    // }
    // connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(doSomething(int)));

    // QWidget *floorButtonContainer = new QWidget;
    // QVBoxLayout *floorButtonLayout = new QVBoxLayout(floorButtonContainer);
    // floorButtonLayout->setAlignment(Qt::AlignBottom);

    // QCheckBox *up, *down;
    // for (int i = 0; i < FLOOR_COUNT; i++) {
    //     up = new QCheckBox("Up");
    //     down = new QCheckBox("Down");
    //     up->setObjectName(QString("floor%1UpButton").arg(i + 1));
    //     down->setObjectName(QString("floor%1DownButton").arg(i + 1));

    //     floorButtonLayout->addWidget(up);
    //     floorButtonLayout->addWidget(down);
    // }

    // ui->horizontalLayout_2->addWidget(floorButtonContainer);

    buttonPressed = false;

    // Update view
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateUi()));
    updateTimer->start(UPDATE_INTERVAL_MS);

    // todo: temp
    Elevator *e = buildingModel->buildingTable[6][0];
    connect(ui->testButton, SIGNAL(released()), this, SLOT(testSlot()));
    connect(this, &MainWindow::testSig, e, &Elevator::elevatorMoving);
}

void MainWindow::doSomething(int i) { emit testSig(Elevator::Direction::UP); }

void MainWindow::testSlot() { emit testSig(Elevator::Direction::UP); }

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
