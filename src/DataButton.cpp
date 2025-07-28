#include "DataButton.h"

#include <QPushButton>
#include <QSizePolicy>
#include <QString>
#include <QWidget>

DataButton::DataButton(bool doDataToggle, bool doPressHold, bool c,
                       QString label)
    : QPushButton(nullptr),  // Parent will be set when button added in UI
      doDataToggle(doDataToggle),
      doPressHold(doPressHold),
      checked(c) {
    // Set text label if one is given
    if (!label.isEmpty()) setText(label);

    // Set size to fill parent widget
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (doDataToggle) {
        // One press will save the state of the button
        connect(this, &DataButton::pressed, this,
                [this]() { this->flipChecked(); });
    } else {
        // Button will only stay checked while pressed down
        connect(this, &DataButton::pressed, this,
                [this]() { this->setChecked(true); });
        connect(this, &DataButton::released, this,
                [this]() { this->setChecked(false); });
    }

    if (doPressHold) {
        // Button will emit its signal repeatedly while pressed
        setAutoRepeat(true);
        setAutoRepeatDelay(autoRepeatMs);
        setAutoRepeatInterval(autoRepeatMs);

        connect(this, &DataButton::released, this,
                [this]() { this->updateStyleSheet(); });
    }
}

bool DataButton::isChecked() const { return checked; }

void DataButton::setChecked(bool newState) {
    if (checked != newState) {
        checked = newState;
        updateStyleSheet();
        emit buttonCheckedUpdate();
    }
}

void DataButton::flipChecked() { setChecked(!checked); }

void DataButton::updateStyleSheet() {
    /** Qt Style Sheet strings for styling the buttons' checked and unchecked
     *  states. */
    const QString checkedStyleSheetStr =
        "background-color: rgba(10, 0, 135, 60%);"
        "color: rgb(255,255,255);";
    const QString unCheckedStyleSheetStr = "";

    // If the button is currently held down, style it as if checked.
    if (checked || (doPressHold && isDown()))
        setStyleSheet(checkedStyleSheetStr);
    else
        setStyleSheet(unCheckedStyleSheetStr);
}
