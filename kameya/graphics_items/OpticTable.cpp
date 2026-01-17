#include "OpticTable.h"
#include "qdatetime.h"

#include <QLinearGradient>
#include <QPainter>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include "QColor"
#include "json_utils.h"
#include "config.h"

QVector<QColor> bulb_colors;

OpticTable::OpticTable()
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    auto init_state = bulb_state::UNDEFINED;
    bulb_state = {init_state,
                  init_state,
                  init_state,
                  init_state,
                  init_state,
                  init_state
                 };

    m_current_lamp_index = 5;
    m_bulb_on_time.resize(6);
    bulb_colors.resize(6);
    QJsonObject jo;
    jsn::getJsonObjectFromFile(global::json_lamps_file_name,jo);
    auto x = jo["lamps_item_coords"].toObject().value("x").toInt();
    auto y = jo["lamps_item_coords"].toObject().value("y").toInt();
    setPos(x,y);
    auto lamps = jo[global::kJsonKeyLampsArray].toArray();
    for(int i=0;i<lamps.size();++i){
        bulb_colors[i] = QColor(lamps[i].toObject()["color"].toString());
    }
}

OpticTable::~OpticTable()
{
    QJsonObject jo;
    jsn::getJsonObjectFromFile(global::json_lamps_file_name,jo);
    auto pos = scenePos().toPoint();
    QJsonObject coords;
    coords["x"] = pos.x();
    coords["y"] = pos.y();
    jo["lamps_item_coords"] = coords;
    jsn::saveJsonObjectToFile(global::json_lamps_file_name,jo,QJsonDocument::Indented);
}

QRectF OpticTable::boundingRect() const
{
    static QImage imgOn(":/guiPictures/bulb_on.png");

    int spacing = imgOn.height() + 10;
    int totalHeight = bulb_state.size() * spacing;
    int totalWidth  = imgOn.width() + 100; // запас справа под время работы

    return QRectF(-10, -10, totalWidth+10, totalHeight+10);
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

    int spacing = imgOn.height() + 10;
    int radius = std::max(imgOn.width(), imgOn.height()) / 2 + 5;

    for (int i = 0; i < bulb_state.size(); ++i) {
        int y = i * spacing;

        // Нарисовать фон-круг
        QBrush back_brush;
        if(bulb_state[i]==bulb_state::ON){
            back_brush = QBrush(Qt::lightGray);
        }else{
            back_brush = QBrush(Qt::darkGray);
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
        if (bulb_state[i] == bulb_state::ON && i < m_bulb_on_time.size()) {
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

bool OpticTable::setBulbOff(int bi)
{
    if(bi>bulb_state.size()-1||bi<0)return true;
    bool is_state_the_same = false;
    if(bulb_state[bi] == bulb_state::OFF){
        is_state_the_same = true;
    }
    bulb_state[bi] = bulb_state::OFF;
    m_current_lamp_index = bi;
    update();
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
    if(!is_state_the_same){
        m_bulb_on_time[bi] = QDateTime::currentDateTime();
    }
    update();
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
    update();
    return is_state_the_same;
}

void OpticTable::set_current_lamp_index(const int index)
{
    if(index < 0 || index > 5) return;
    m_current_lamp_index = index;
    update();
}

void OpticTable::set_bulb_states(QVector<enum class bulb_state> states)
{
    bulb_state = states;
    update();
}

