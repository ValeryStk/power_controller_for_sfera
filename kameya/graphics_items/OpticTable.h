#ifndef OPTIC_TABLE_H
#define OPTIC_TABLE_H

#include <QGraphicsItem>

enum class bulb_state{ON,OFF,UNDEFINED};

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
    void set_current_lamp_index(const int index);
    void set_bulb_states(QVector<bulb_state> states);

private:

    QVector<bulb_state> bulb_state;
    void drawLamps(QPainter *painter);
    int m_current_lamp_index = 0;
    QVector<QDateTime> m_bulb_on_time;

};

#endif // OPTIC_TABLE_H
