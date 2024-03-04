#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollBar>
#include <QStringBuilder>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    static const int ELEVATOR_COUNT;

private slots:
    void testFunction();
    void inlineConsoleDisplay(const QString &text);
};
#endif // MAINWINDOW_H
