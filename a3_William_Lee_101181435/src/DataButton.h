#ifndef DATABUTTON_H
#define DATABUTTON_H

#include <QPushButton>
#include <QString>
#include <QWidget>

/** Customizable button for holding data and informing of changes.
 *
 * Custom subclass of QPushButton, adding a common interface to handle a button
 * that is exposed in the UI, can be toggled or only emit a signal while it is
 * pressed down, and can emit signals continuously while pressed down.
 *
 * Data Members:
 * # autoRepeatMs: int
 *      Interval for emitting pressed signal when the button is held down
 *      continuously, in milliseconds.
 * # doDataToggle: bool
 *      Whether button should remember its state when pressed in the UI like a
 *      toggle (true), or if it should only stay checked while being pressed.
 * # doPressHold: bool
 *      Whether button should emit its buttonCheckedUpdate() signal once when
 *      pressed (true), or repeatedly while pressed down.
 * # checked: bool
 *      Current checked state of the button.
 *
 * Class Methods:
 * + isChecked(): bool
 *      Returns true if the button is currently checked.
 * + setChecked(bool): void
 *      Set checked state to supplied boolean.
 * + flipChecked(): void
 *      Invert the current checked state.
 * # updateStyleSheet(): void (abstract)
 *      Sets a Qt style sheet on the button according to current button state.
 *      May be overridden by subclasses.
 *
 * Signals:
 * + buttonCheckedUpdate(): void
 *      Emitted to inform of the checked status of the button.
 */
class DataButton : public QPushButton {
    Q_OBJECT

   public:
    explicit DataButton(bool doDataToggle = true, bool doPressHold = true,
                        bool initialChecked = false, QString objectName = "",
                        QWidget *parent = nullptr);

    /* Public methods */
    bool isChecked() const;
    void setChecked(bool);
    void flipChecked();

   signals:
    void buttonCheckedUpdate();

   protected:
    /* Protected data members */
    static const int autoRepeatMs = 1000;  // 1 second
    const bool doDataToggle;
    const bool doPressHold;
    bool checked;

    /* Protected methods */
    virtual void updateStyleSheet();
};

#endif /* DATABUTTON_H */
