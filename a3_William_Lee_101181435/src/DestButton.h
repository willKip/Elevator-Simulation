#ifndef DESTBUTTON_H
#define DESTBUTTON_H
#include <QObject>
#include <QString>

#include "DataButton.h"

class DestButton : public DataButton {
    Q_OBJECT

   public:
    explicit DestButton(int destFloorNum, QString objectName = "",
                        QWidget *parent = nullptr);

    const int destFloorNum;
};

#endif /* DESTBUTTON_H */
