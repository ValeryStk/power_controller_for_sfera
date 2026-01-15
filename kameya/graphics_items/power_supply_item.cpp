// PowerSupplyItem.cpp
#include "power_supply_item.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QSvgRenderer>
#include <QFontMetricsF>
#include <QToolTip>
#include <QDebug>
#include <QJsonObject>
#include "json_utils.h"

constexpr char json_ini_name[] = "ir_lamps.json";

PowerSupplyItem::PowerSupplyItem(const QString& svgPath,
                                 const QString& name,
                                 const QString& obj_name):m_label(name),
                                                          m_object_name(obj_name)
{

    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);

    if (!svgPath.isEmpty()) {
        loadSvg(svgPath);
    }
    QJsonObject jo;
    jsn::getJsonObjectFromFile(json_ini_name,jo);
    auto x = jo[m_object_name].toObject().value("x").toInt();
    auto y = jo[m_object_name].toObject().value("y").toInt();
    setPos(x,y);
}

PowerSupplyItem::~PowerSupplyItem()
{
    QJsonObject jo;
    jsn::getJsonObjectFromFile(json_ini_name,jo);
    auto pos = scenePos().toPoint();
    QJsonObject coords = jo[m_object_name].toObject();
    coords["x"] = pos.x();
    coords["y"] = pos.y();
    jo[m_object_name] = coords;
    jsn::saveJsonObjectToFile(json_ini_name,jo,QJsonDocument::Indented);
}

bool PowerSupplyItem::loadSvg(const QString& svgPath)
{
    setSharedRenderer(new QSvgRenderer(svgPath));
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
    painter->setPen(QPen(Qt::lightGray));
    painter->drawText(QPointF(165,30),m_label);

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
    update();
}

void PowerSupplyItem::set_out_1_active()
{
    m_is_out_1_active = true;
    update();
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
    update();
}

void PowerSupplyItem::set_out_2_active()
{
    m_is_out_2_active = true;
    update();
}

void PowerSupplyItem::set_all_outs_unactive()
{
    m_is_out_1_active = false;
    m_is_out_2_active = false;
    update();
}

void PowerSupplyItem::setLabel(const QString& name)
{
    if (m_label == name) return;
    m_label = name;
    update();
}

