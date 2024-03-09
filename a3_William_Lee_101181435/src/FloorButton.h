#ifndef FLOORBUTTON_H
#define FLOORBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QString>

#include "Building.h"
#include "Direction.h"

/**
 * Custom subclass of QPushButton that can appear to be checkable, but instead
 * fires a signal to the underlying data structure and reflects the appropriate
 * state. For this, it ignores the default "checked" state of QPushButton and
 * implements its own.
 */
// TODO: documentation
class FloorButton : public QPushButton {
    Q_OBJECT

   public:
    FloorButton(int floorNum, Direction direction, bool initialChecked = false,
                QString objectName = "", QWidget *parent = nullptr);

    bool isChecked() const;
    void setChecked(bool);

    Direction getDirection();
    int getFloorNum();

   private:
    /**
     * Qt Style Sheet strings for styling the buttons' checked and unchecked
     * states.
     */
    static const QString checkedStyleSheetStr;
    static const QString unCheckedStyleSheetStr;

    const int floorNum;
    const Direction direction;
    bool checked;

    // Provide mapping of Direction enum to text for the button label.
    QString directionToLabelText() const;
    void updateStyleSheet();
};
#endif /* FLOORBUTTON_H */
