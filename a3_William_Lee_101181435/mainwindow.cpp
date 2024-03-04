#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->testButton, SIGNAL(released()), this, SLOT(testFunction()));

    // TODO: Init elevators and floors here
    qInfo("int = " + ELEVATOR_COUNT);

    // Initialize number of loops since simulation start.
    timeCount = 0;

    // Update view
    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateUi()));
    updateTimer->start(UPDATE_INTERVAL_MS);

    buttonPressed = false;
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
