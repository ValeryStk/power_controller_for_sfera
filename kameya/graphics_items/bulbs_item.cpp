#include "bulbs_item.h"

#include <QLinearGradient>
#include <QPainter>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include "QColor"
#include "json_utils.h"

namespace {
bool is_index_invalid(const int index){
    return (index < 0 || index > MAX_CURRENT_LAMP_INDEX);
};
}//end namespace

BulbsQGraphicsItem::BulbsQGraphicsItem()
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    m_current_lamp_index = MAX_CURRENT_LAMP_INDEX;
    QJsonObject jo;
    jsn::getJsonObjectFromFile(global::json_lamps_file_name,jo);
    auto coords = jo[global::kJsonKeyLampsCoordsObject].toObject();
    auto x = coords.value(global::kJsonKeyX).toInt();
    auto y = coords.value(global::kJsonKeyY).toInt();
    setPos(x,y);
    auto lamps = jo[global::kJsonKeyLampsArray].toArray();
    for(int i=0;i<NUMBER_OF_LAMPS;++i){
        bulb_colors[i] = QColor(lamps[i].toObject()[global::kJsonKeyColor].toString());
        bulb_states[i] = bulb_state::UNDEFINED;
    }
}

BulbsQGraphicsItem::~BulbsQGraphicsItem()
{
    QJsonObject jo;
    jsn::getJsonObjectFromFile(global::json_lamps_file_name,jo);
    auto pos = scenePos().toPoint();
    QJsonObject coords;
    coords[global::kJsonKeyX] = pos.x();
    coords[global::kJsonKeyY] = pos.y();
    jo[global::kJsonKeyLampsCoordsObject] = coords;
    jsn::saveJsonObjectToFile(global::json_lamps_file_name,jo,QJsonDocument::Indented);
}

QRectF BulbsQGraphicsItem::boundingRect() const
{
    static QImage imgOn(":/guiPictures/bulb_on.png");
    int spacing = imgOn.height() + 10;
    int totalHeight = NUMBER_OF_LAMPS * spacing;
    int totalWidth  = imgOn.width() + 100; // запас справа под время работы
    return QRectF(-10, -10, totalWidth+10, totalHeight+10);
}


void BulbsQGraphicsItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    drawLamps(painter);
}

void BulbsQGraphicsItem::drawLamps(QPainter *painter)
{

    static QImage imgOn(":/guiPictures/bulb_on.png");
    static QImage imgOff(":/guiPictures/bulb_off.png");
    static QImage imgUndefined(":/guiPictures/bulb_undefined.png");

    int spacing = imgOn.height() + 10;
    int radius = std::max(imgOn.width(), imgOn.height()) / 2 + 5;

    for (int i = 0; i < NUMBER_OF_LAMPS; ++i) {
        int y = i * spacing;

        // Нарисовать фон-круг
        QBrush back_brush;
        if(bulb_states[i]==bulb_state::ON){
            back_brush = QBrush(Qt::lightGray);
        }else{
            back_brush = QBrush(Qt::darkGray);
        }
        painter->setBrush(back_brush);
        painter->setPen(Qt::NoPen);
        // без обводки
        int centerX = imgOn.width() / 2; int centerY = y + imgOn.height() / 2;
        painter->drawEllipse(QPoint(centerX, centerY), radius, radius);

        switch (bulb_states[i]) {
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


        // --- вывод порядкового номера лампы в кружке фона ---
        painter->setPen(Qt::black);
        QFont font = painter->font();
        font.setBold(true);
        font.setPointSize(10);
        painter->setFont(font);

        // смещение для "19 часов" (примерно влево и вниз от центра круга)
        int offsetX = -radius * 0.7;
        int offsetY = radius * 0.4;

        // прямоугольник для текста
        QRect textRect(centerX + offsetX - 10, centerY + offsetY - 10, 20, 20);

        // рисуем номер лампы (i+1)
        painter->drawText(textRect, Qt::AlignCenter, QString::number(i + 1));

        offsetX = radius * 0.8;
        offsetY = radius * 0.8;
        painter->setBrush(bulb_colors[i]);
        painter->drawEllipse(QPoint(centerX + offsetX - 10, centerY + offsetY - 10),8,8);

        // если это текущая лампа — обводим красным жирным кругом
        if (i == m_current_lamp_index) {
            // currentLampIndex — индекс текущей лампы
            QPen pen(bulb_colors[i]);
            pen.setWidth(4);
            // толщина линии
            painter->setPen(pen);
            painter->setBrush(Qt::NoBrush);
            painter->drawEllipse(QPoint(centerX, centerY), radius, radius);
        }

        // --- вывод времени работы лампы справа ---
        if (bulb_states[i] == bulb_state::ON) {
            // вычисляем прошедшее время
            qint64 secs = m_bulb_on_time[i].secsTo(QDateTime::currentDateTime());
            int minutes = secs / 60;
            int seconds = secs % 60;

            QString timeText = QString("%1:%2")
                    .arg(minutes, 2, 10, QChar('0'))
                    .arg(seconds, 2, 10, QChar('0'));

            painter->setPen(Qt::lightGray);
            QFont font = painter->font();
            font.setPointSize(11);
            painter->setFont(font);

            // прямоугольник справа от круга
            QRect timeRect(centerX + radius + 10, centerY - 10, 50, 20);
            painter->drawText(timeRect, Qt::AlignLeft | Qt::AlignVCenter, timeText);
        }
    }
}

bool BulbsQGraphicsItem::setBulbOff(int bi)
{
    if(is_index_invalid(bi))return true;
    m_current_lamp_index = bi;
    if(bulb_states[bi] == bulb_state::OFF){
        return true;
    }
    bulb_states[bi] = bulb_state::OFF;
    update();
    return false;
}

bool BulbsQGraphicsItem::setBulbOn(int bi)
{
    if(is_index_invalid(bi))return true;
    m_current_lamp_index = bi;
    if(bulb_states[bi] == bulb_state::ON){
        return true;
    }
    bulb_states[bi] = bulb_state::ON;
    m_bulb_on_time[bi] = QDateTime::currentDateTime();
    update();
    return false;
}

bool BulbsQGraphicsItem::setBulbUndefined(int bi)
{
    if(is_index_invalid(bi)) return true;
    m_current_lamp_index = bi;
    if(bulb_states[bi] == bulb_state::UNDEFINED){
        return true;
    }
    bulb_states[bi] = bulb_state::UNDEFINED;
    update();
    return false;
}

void BulbsQGraphicsItem::set_current_lamp_index(const int index)
{
    if(is_index_invalid(index)) return;
    m_current_lamp_index = index;
    update();
}

void BulbsQGraphicsItem::set_bulb_states(enum class bulb_state states[])
{
    std::copy(states, states + NUMBER_OF_LAMPS, bulb_states);
    update();
}
