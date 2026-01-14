// PowerSupplyItem.cpp
#include "power_supply_item.h"

#include "qgraphicssceneevent.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QSvgRenderer>
#include <QFontMetricsF>
#include <QToolTip>
#include <QDebug>
#include <QJsonObject>
#include "json_utils.h"

PowerSupplyItem::PowerSupplyItem(const QString& svgPath,
                                 const QString& name):m_label(name)
{
    // Improve rendering quality
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true); // включаем события наведения
    if (!svgPath.isEmpty()) {
        loadSvg(svgPath);
    }
    QJsonObject jo;
    jsn::getJsonObjectFromFile("ir_lamps.json",jo);
    auto x = jo[m_label].toObject().value("x").toInt();
    auto y = jo[m_label].toObject().value("y").toInt();
    setPos(x,y);
}

PowerSupplyItem::~PowerSupplyItem()
{
    QJsonObject jo;
    auto pos = scenePos().toPoint();
    QJsonObject coords;
    coords["x"] = pos.x();
    coords["y"] = pos.y();
    jsn::getJsonObjectFromFile("ir_lamps.json",jo);
    jo[m_label] = coords;
    jsn::saveJsonObjectToFile("ir_lamps.json",jo,QJsonDocument::Indented);
    qDebug()<<"-------------JO-----------------"<<jo;
}

bool PowerSupplyItem::loadSvg(const QString& svgPath)
{
    setSharedRenderer(new QSvgRenderer(svgPath));
    // Update bounds cache
    m_cachedBounds = QGraphicsSvgItem::boundingRect();
    update();
    return renderer() && renderer()->isValid();
}

QRectF PowerSupplyItem::boundingRect() const
{
    // Use SVG bounds; could be expanded if overlay exceeds it
    return m_cachedBounds.isEmpty() ? QGraphicsSvgItem::boundingRect() : m_cachedBounds;
}

void PowerSupplyItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // First, paint the SVG
    QGraphicsSvgItem::paint(painter, option, widget);

    painter->setRenderHint(QPainter::Antialiasing, true);

    // Prepare text overlay
    painter->setFont(m_font);

    // Lines: label, voltage, current, state
    const QString lineVoltage = QStringLiteral("%1 V").arg(m_voltage_out_1, 0, 'f', 3);
    const QString lineCurrent = QStringLiteral("%1 A").arg(m_current_out_1, 0, 'f', 3);

    const QString lineVoltage2 = QStringLiteral("%1 V").arg(m_voltage_out_2, 0, 'f', 3);
    const QString lineCurrent2 = QStringLiteral("%1 A").arg(m_current_out_2, 0, 'f', 3);


    // Draw text
    painter->setPen(m_textColor);
    painter->drawText(QPointF(100,70), lineVoltage);
    painter->drawText(QPointF(100,120), lineCurrent);

    painter->drawText(QPointF(331,70), lineVoltage2);
    painter->drawText(QPointF(331,120), lineCurrent2);
    //painter->drawText(QPointF(165,20),"192.168.1.22");

    // State line with color
    QColor stateColor = m_enabled_out_1 ? m_enabledColorOn : m_enabledColorOff;
    painter->setPen(stateColor);

    // Optional status indicator circle (top-right)
    const qreal r = 8.0;
    QPointF circleCenter(240, 140);
    QPointF circleCenter2(475, 140);
    painter->setPen(Qt::NoPen);
    painter->setBrush(stateColor);
    painter->drawEllipse(circleCenter, r, r);
    painter->drawEllipse(circleCenter2, r, r);

    painter->setBrush(QBrush(m_out_1_color));
    painter->setPen(QPen(m_out_1_color,5));
    painter->drawEllipse(QPointF(202,270),8,8);
    painter->drawEllipse(QPointF(250,270),8,8);
    if(m_is_out_1_active){
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(QRect(QPoint(177,247),QPoint(273,288)));
    }

    painter->setBrush(QBrush(m_out_2_color));
    painter->setPen(QPen(m_out_2_color,5));
    painter->drawEllipse(QPointF(350,270),8,8);
    painter->drawEllipse(QPointF(398,270),8,8);
    if(m_is_out_2_active){
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(QRect(QPoint(321,247),QPoint(425,288)));
    }

}

void PowerSupplyItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{

        // Локальные координаты внутри блока питания
        QPointF localPos = event->pos();

        // Глобальные координаты сцены
        QPointF scenePos = event->scenePos();

        // Показываем всплывающую подсказку с координатами
        QString text = QString("Local: (%1, %2)\nScene: (%3, %4)")
                           .arg(localPos.x()).arg(localPos.y())
                           .arg(scenePos.x()).arg(scenePos.y());
        qDebug()<<text;
}

void PowerSupplyItem::set_voltage_out_1(double volts)
{
    if (qFuzzyCompare(m_voltage_out_1, volts)) return;
    m_voltage_out_1 = volts;
    update();
}

void PowerSupplyItem::set_current_out_1(double amps)
{
    if (qFuzzyCompare(m_current_out_1, amps)) return;
    m_current_out_1 = amps;
    update();
}

void PowerSupplyItem::set_enabled_out_1(bool on)
{
    if (m_enabled_out_1 == on) return;
    m_enabled_out_1 = on;
    update();
}

void PowerSupplyItem::set_out_1_color(const QColor &color)
{
    m_out_1_color = color;
}

void PowerSupplyItem::set_out_1_active()
{
    m_is_out_1_active = true;
}

void PowerSupplyItem::set_voltage_out_2(double volts)
{
    if (qFuzzyCompare(m_voltage_out_2, volts)) return;
    m_voltage_out_2 = volts;
    update();
}

void PowerSupplyItem::set_current_out_2(double amps)
{
    if (qFuzzyCompare(m_current_out_2, amps)) return;
    m_current_out_2 = amps;
    update();
}

void PowerSupplyItem::set_enabled_out_2(bool on)
{
    if (m_enabled_out_2 == on) return;
    m_enabled_out_2 = on;
    update();
}

void PowerSupplyItem::set_out_2_color(const QColor &color)
{
    m_out_2_color = color;
}

void PowerSupplyItem::set_out_2_active()
{
    m_is_out_2_active = true;
}

void PowerSupplyItem::set_all_outs_unactive()
{
    m_is_out_1_active = false;
    m_is_out_2_active = false;
}

void PowerSupplyItem::setLabel(const QString& name)
{
    if (m_label == name) return;
    m_label = name;
    update();
}

void PowerSupplyItem::setTextColor(const QColor& color)
{
    if (m_textColor == color) return;
    m_textColor = color;
    update();
}

void PowerSupplyItem::setEnabledColor(const QColor& onColor, const QColor& offColor)
{
    bool changed = false;
    if (m_enabledColorOn != onColor) { m_enabledColorOn = onColor; changed = true; }
    if (m_enabledColorOff != offColor) { m_enabledColorOff = offColor; changed = true; }
    if (changed) update();
}

void PowerSupplyItem::setFont(const QFont& font)
{
    if (m_font == font) return;
    m_font = font;
    update();
}
