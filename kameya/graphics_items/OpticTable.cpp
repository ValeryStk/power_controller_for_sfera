#include "OpticTable.h"

#include <QLinearGradient>
#include <QPainter>
#include <QDebug>



OpticTable::OpticTable()
{
    auto init_state = bulb_state::UNDEFINED;
    bulb_state = {init_state,
                  init_state,
                  init_state,
                  init_state,
                  init_state,
                  init_state
                 };
    m_current_lamp_index = 5;
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
    static QImage imgUndefined(":/guiPictures/bulb_undefined.png");

    int spacing = imgOn.height() + 10; // vertical spacing between bulbs
    int radius = std::max(imgOn.width(), imgOn.height()) / 2 + 5; // радиус круга чуть больше лампы

    for (int i = 0; i < bulb_state.size(); ++i) {
        int y = i * spacing;

        // Нарисовать фон-круг
        QBrush back_brush;
        if(bulb_state[i]==bulb_state::ON){
            back_brush = QBrush(Qt::darkGray);
        }else{
            back_brush = QBrush(Qt::lightGray);
        }
        painter->setBrush(back_brush);
        painter->setPen(Qt::NoPen);
        // без обводки
        int centerX = imgOn.width() / 2; int centerY = y + imgOn.height() / 2;
        painter->drawEllipse(QPoint(centerX, centerY), radius, radius);

        switch (bulb_state[i]) {
        case bulb_state::ON:{
            painter->drawImage(0, y, imgOn);
            break;
        }
        case bulb_state::OFF:{
            painter->drawImage(0, y, imgOff);
            break;
        }
        case bulb_state::UNDEFINED:{
            painter->drawImage(0, y, imgUndefined);
            break;
        }
        }


        // если это текущая лампа — обводим красным жирным кругом
        if (i == m_current_lamp_index) {
            // currentLampIndex — индекс текущей лампы
            QPen pen(Qt::red);
            pen.setWidth(4);
            // толщина линии
            painter->setPen(pen);
            painter->setBrush(Qt::NoBrush);
            painter->drawEllipse(QPoint(centerX, centerY), radius, radius);
        }
    }

}

bool OpticTable::setBulbOff(int bi)
{
    if(bi>bulb_state.size()-1||bi<0)return true;
    bool is_state_the_same = false;
    if(bulb_state[bi] == bulb_state::OFF){
        is_state_the_same = true;
    }
    bulb_state[bi] = bulb_state::OFF;
    m_current_lamp_index = bi;
    return is_state_the_same;
}

bool OpticTable::setBulbOn(int bi)
{
    if(bi>bulb_state.size()-1||bi<0)return true;
    bool is_state_the_same = false;
    if(bulb_state[bi] == bulb_state::ON){
        is_state_the_same = true;
    }
    bulb_state[bi] = bulb_state::ON;
    m_current_lamp_index = bi;
    return is_state_the_same;
}

bool OpticTable::setBulbUndefined(int bi)
{
    if(bi>bulb_state.size()-1||bi<0)return true;
    bool is_state_the_same = false;
    if(bulb_state[bi] == bulb_state::UNDEFINED){
        is_state_the_same = true;
    }
    bulb_state[bi] = bulb_state::UNDEFINED;
    m_current_lamp_index = bi;
    return is_state_the_same;
}

void OpticTable::set_current_lamp_index(const int index)
{
    if(index < 0 || index > 5) return;
    m_current_lamp_index = index;
}
