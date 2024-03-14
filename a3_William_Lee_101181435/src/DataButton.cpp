#include "DataButton.h"

#include <QPushButton>
#include <QSizePolicy>
#include <QString>

// TODO: cleanup

// Static initialization
const int DataButton::autoRepeatMs = 1000;  // 1 second

DataButton::DataButton(bool doDataToggle, bool doPressHold, bool c,
                       QString objectName, QWidget *parent)
    : QPushButton(parent), doPressHold(doPressHold), checked(c) {
    // Set Qt button object name
    if (!objectName.isEmpty()) setObjectName(objectName);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (doDataToggle) {
        connect(this, &DataButton::pressed, this,
                [this]() { this->flipChecked(); });
    } else {
        connect(this, &DataButton::pressed, this,
                [this]() { this->setChecked(true); });
        connect(this, &DataButton::released, this,
                [this]() { this->setChecked(false); });
    }

    if (doPressHold) {
        // Set up button pressed and held behaviour
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
        emit buttonCheckedChanged();
    }
}

void DataButton::flipChecked() { setChecked(!checked); }

void DataButton::updateStyleSheet() {
    /**
     * Qt Style Sheet strings for styling the buttons' checked and unchecked
     * states.
     */
    const QString checkedStyleSheetStr =
        "background-color: rgba(10, 0, 135, 60%);"
        "color: rgb(255,255,255);";
    const QString unCheckedStyleSheetStr = "";

    // If the button is currently being held down, style it as if checked.
    if (checked || (doPressHold && isDown()))
        setStyleSheet(checkedStyleSheetStr);
    else
        setStyleSheet(unCheckedStyleSheetStr);
}
