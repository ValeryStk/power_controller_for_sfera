#ifndef OPTICTABLE_H
#define OPTICTABLE_H

#include <QGraphicsItem>


class OpticTable:public QGraphicsItem
{
public:
    OpticTable();

public:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setBulbOff(int bi);

private:
    QVector<bool> isBulbsOn;
    void drawLamps(QPainter *painter);

};

#endif // OPTICTABLE_H
