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

void OpticTable::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::HighQualityAntialiasing);
    drawLamps(painter);
    /*painter->translate(boundingRect().width() / 2, boundingRect().height() / 2);
    QRectF br = boundingRect();
    QPen pen;
    pen.setColor(QColor(128,0,0,128));
    pen.setWidth(2);
    painter->setBrush(QColor(255,0,0,128));
    painter->drawEllipse(-(br.width()/4/2),-(br.width()/4/2),br.width()/4,br.height()/4);

    static const QPoint hourHand[3] = {

        QPoint(4, -br.height()),
        QPoint(8, 0),
        QPoint(-8, 0),

    };
    painter->save();
    painter->rotate(m_angles.leftSafety);
    painter->setBrush(QColor(255,0,0,220));
    painter->drawRect(-10,-br.width(),20,20);

    painter->restore();
    painter->save();
    painter->rotate(m_angles.rightSafety);
    painter->drawRect(-10,-br.width(),20,20);

    painter->restore();
    painter->save();
    painter->rotate(m_angles.bigSphere);
    painter->setBrush(QColor(255,233,0,220));
    painter->drawEllipse(-30,-br.width(),60,60);

    painter->restore();
    painter->save();
    painter->rotate(m_angles.smallSphere);
    painter->setBrush(QColor(255,127,39,220));
    painter->drawEllipse(-30,-br.width(),60,60);

    painter->restore();
    painter->save();
    painter->rotate(m_angles.linearLight);
    painter->setBrush(QColor(132,61,186,200));
    painter->drawEllipse(-30,-br.width(),60,60);

    painter->restore();
    painter->save();
    painter->setBrush(QColor("yellow"));
    painter->rotate(angle);
    painter->drawConvexPolygon(hourHand,3);

    painter->restore();*/


}

void OpticTable::setAngle(int newAngle)
{
    angle = newAngle;
}

void OpticTable::setAngles(AnglesScene &newAngles)
{
    m_angles = newAngles;
}

void OpticTable::drawLamps(QPainter *painter)
{
    QImage imgOn(":/guiPictures/bulb_on.png");
    QImage imgOff(":/guiPictures/bulb_off.png");
    for(int i=0;i<isBulbsOn.size();++i){

        if(isBulbsOn[i])painter->drawImage(0,(i*imgOn.width()-50),imgOn);
        else {
            painter->drawImage(0,(i*imgOff.width()+20),imgOff);
        }
    }
}

void OpticTable::setBulbOff(int bi)
{
    if(bi>isBulbsOn.size()-1||bi<0)return;
    isBulbsOn[bi] = false;
    qDebug()<<bi<<"************************";
}
