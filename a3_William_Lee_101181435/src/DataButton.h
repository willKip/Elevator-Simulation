#ifndef DATABUTTON_H
#define DATABUTTON_H

#include <QObject>
#include <QPushButton>

/**
 * Custom subclass of QPushButton that can appear to be checkable, but
 * instead fires a signal to the underlying data structure and reflects the
 * appropriate state. For this, it ignores the default "checked" state of
 * QPushButton and implements its own.
 */
// TODO: documentation
class DataButton : public QPushButton {
    Q_OBJECT

   public:
    explicit DataButton(bool initialChecked = false, QString objectName = "",
                        QWidget *parent = nullptr);

    bool isChecked() const;  // Return current checked state
    void setChecked(bool);   // Set checked to given bool
    void flipChecked();      // Invert current checked state

   signals:
    // Fired when the button is pressed in the UI.
    // Underlying data must be updated by the related data structures by
    // capturing this signal.
    void buttonCheckedChanged();

   protected:
    bool checked;

    // Default provided, but can be overridden by subclasses to specify styling
    // based on specific and/or additional button states
    virtual void updateStyleSheet();
};

#endif /* DATABUTTON_H */
