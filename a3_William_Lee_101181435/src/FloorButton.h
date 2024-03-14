#ifndef FLOORBUTTON_H
#define FLOORBUTTON_H

#include <QString>
#include <QWidget>

#include "DataButton.h"
#include "Direction.h"

// TODO: documentation
// TODO: cleanup
class FloorButton : public DataButton {
    Q_OBJECT

   public:
    explicit FloorButton(int floorNum, Direction direction,
                         bool initialChecked = false, QString objectName = "",
                         QWidget *parent = nullptr);

   private:
    const int floorNum;
    const Direction direction;

    // Provide mapping of Direction enum to text for the button label.
    QString directionToLabelText() const;

    // Override of base DataButton styling
    void updateStyleSheet() override;
};
#endif /* FLOORBUTTON_H */
