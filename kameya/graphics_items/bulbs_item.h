#ifndef BULBS_QGRAPHICS_ITEM_H
#define BULBS_QGRAPHICS_ITEM_H

#include <QDateTime>
#include <QElapsedTimer>
#include <QGraphicsItem>

#include "config.h"

enum class bulb_state { ON, OFF, UNDEFINED };

class BulbsQGraphicsItem : public QGraphicsItem {
public:
    BulbsQGraphicsItem();
    ~BulbsQGraphicsItem() override;

public:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    bool setBulbOff(int bi);
    bool setBulbOn(int bi);
    bool setBulbUndefined(int bi);
    void set_current_lamp_index(const int index);
    void set_bulb_states(bulb_state states[]);

private:
    void drawLamps(QPainter *painter);
    int m_current_lamp_index = 0;
    bulb_state bulb_states[NUMBER_OF_LAMPS];
    QColor bulb_colors[NUMBER_OF_LAMPS];
    QElapsedTimer m_bulb_on_time[NUMBER_OF_LAMPS];
};

#endif  // BULBS_QGRAPHICS_ITEM_H
