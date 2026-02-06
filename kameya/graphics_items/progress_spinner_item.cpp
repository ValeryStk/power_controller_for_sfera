#include "progress_spinner_item.h"

SpinnerItem::SpinnerItem(QGraphicsItem *parent)
    : QGraphicsItem(parent),
      m_outerRotation(0),
      m_innerRotation(0),
      m_opacity(1.0),
      m_fadeOut(true) {
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

SpinnerItem::~SpinnerItem() {}

QRectF SpinnerItem::boundingRect() const { return QRectF(-70, -70, 140, 140); }

void SpinnerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                        QWidget *) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->save();
    painter->translate(boundingRect().center());

    // --- Внешний круг ---
    painter->save();
    painter->rotate(m_outerRotation);
    QPen outerPen(QColor(180, 180, 255), 4);
    painter->setPen(outerPen);
    painter->setOpacity(0.6);
    painter->drawEllipse(QRectF(-60, -60, 120, 120));
    painter->restore();

    // --- Внутренняя дуга ---
    painter->save();
    painter->rotate(m_innerRotation);
    QPen innerPen(QColor(100, 150, 255), 8, Qt::SolidLine, Qt::RoundCap);
    painter->setPen(innerPen);
    painter->setOpacity(m_opacity);

    QRectF circleRect(-40, -40, 80, 80);
    int startAngle = 0;        // начальный угол
    int spanAngle = 100 * 16;  // длина дуги (100°)
    painter->drawArc(circleRect, startAngle, spanAngle);
    painter->restore();

    painter->restore();
}

void SpinnerItem::rotate() { advance(0); }

void SpinnerItem::stop() {
    m_outerRotation = 0;
    m_innerRotation = 0;
    m_opacity = 1.0;
    update();
}

void SpinnerItem::advance(int) {
    // внешний круг вращается по часовой стрелке
    m_outerRotation += 2;
    if (m_outerRotation >= 360) m_outerRotation -= 360;

    // внутренняя дуга вращается против часовой стрелки
    m_innerRotation -= 8;
    if (m_innerRotation <= -360) m_innerRotation += 360;

    // плавное затухание/появление дуги
    if (m_fadeOut) {
        m_opacity -= 0.03;
        if (m_opacity <= 0.2) {
            m_opacity = 0.2;
            m_fadeOut = false;
        }
    } else {
        m_opacity += 0.03;
        if (m_opacity >= 1.0) {
            m_opacity = 1.0;
            m_fadeOut = true;
        }
    }

    update();
}
