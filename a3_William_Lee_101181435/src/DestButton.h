#ifndef DESTBUTTON_H
#define DESTBUTTON_H

#include <QString>

#include "DataButton.h"

// TODO: doc
class DestButton : public DataButton {
    Q_OBJECT

   public:
    explicit DestButton(int destFloorNum, QString objectName = "",
                        QWidget *parent = nullptr);

    const int destFloorNum;
};

#endif /* DESTBUTTON_H */
