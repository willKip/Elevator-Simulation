#include "DataButton.h"

#include <QObject>
#include <QPushButton>
#include <QSizePolicy>
#include <QString>

DataButton::DataButton(bool c, QString objectName, QWidget *parent)
    : QPushButton(parent), checked(c) {
    // Set Qt button object name
    if (!objectName.isEmpty()) setObjectName(objectName);

    // Set style for initial button state
    updateStyleSheet();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(this, &DataButton::pressed, this,
            [this]() { this->flipChecked(); });
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

    if (checked)
        setStyleSheet(checkedStyleSheetStr);
    else
        setStyleSheet(unCheckedStyleSheetStr);
}
