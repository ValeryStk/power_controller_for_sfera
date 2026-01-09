#ifndef OPTIC_TABLE_H
#define OPTIC_TABLE_H

#include <QGraphicsItem>


class OpticTable:public QGraphicsItem
{
public:
    OpticTable();

public:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    bool setBulbOff(int bi);
    bool setBulbOn(int bi);
    bool setBulbUndefined(int bi);

private:
    enum class bulb_state{ON,OFF,UNDEFINED};
    QVector<bulb_state> bulb_state;
    void drawLamps(QPainter *painter);
    int m_current_lamp_index = 0;

};

#endif // OPTIC_TABLE_H
