#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QBoxLayout>
#include <QMainWindow>
#include <QTableView>
#include <QVector>
#include <QWidget>

class Building;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/** Main Qt Window
 *
 * Data Members:
 * - FLOOR_COUNT: int
 * - ELEVATOR_COUNT: int
 *      Program constants. GUI can accommodate for any number of floors and
 *      elevators (minimum 1 for each). Must be modified before compiling.
 *
 * - ui: Ui::MainWindow *
 *      Qt MainWindow object.
 *
 * - buildingModel: Building *
 * - buildingView: QTableView *
 *      Model/View for Building to be displayed in the main window.
 *
 * Class Methods:
 * - addIndexWidgets(int rowIndex, int colIndex,
 *                   QVector<QWidget *> widgetsToAdd,
 *                   QBoxLayout::Direction layoutType): void
 *      Adds widgets to the building view at the specified index.
 *      Horizontal layout unless specified otherwise.
 *
 * Slots:
 * - inlineConsoleDisplay(const QString &): void
 *      Prints specified text to the text log in the UI.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

   private:
    /* PROGRAM CONSTANTS */
    // GUI can accommodate any number of floors or elevators, must be recompiled
    // every time consts are changed.
    static const int FLOOR_COUNT = 7;
    static const int ELEVATOR_COUNT = 3;

    /* Private data members */
    Ui::MainWindow *ui;
    Building *buildingModel;
    QTableView *buildingView;

    /* Private methods */
    void addIndexWidgets(
        int rowIndex, int colIndex, QVector<QWidget *> widgetsToAdd,
        QBoxLayout::Direction layoutType = QBoxLayout::Direction::LeftToRight);

   private slots:
    void inlineConsoleDisplay(const QString &text);
};
#endif  // MAINWINDOW_H
