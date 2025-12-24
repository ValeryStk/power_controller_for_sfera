#include "OpticTable.h"

#include <QLinearGradient>
#include <QPainter>
#include <QDebug>



OpticTable::OpticTable()
{
    isBulbsOn = {true,true,true,true,true,true};
}

QRectF OpticTable::boundingRect() const
{
    return QRectF(0, 0, 400, 400);
}

void OpticTable::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    drawLamps(painter);
}

void OpticTable::drawLamps(QPainter *painter)
{

        static QImage imgOn(":/guiPictures/bulb_on.png");
        static QImage imgOff(":/guiPictures/bulb_off.png");

        int spacing = imgOn.height() + 10; // vertical spacing between bulbs

        for (int i = 0; i < isBulbsOn.size(); ++i) {
            int y = i * spacing;
            if (isBulbsOn[i]) {
                painter->drawImage(0, y, imgOn);
            } else {
                painter->drawImage(0, y, imgOff);
            }
        }

}

void OpticTable::setBulbOff(int bi)
{
    if(bi>isBulbsOn.size()-1||bi<0)return;
    isBulbsOn[bi] = false;
}
