// PowerSupplyItem.h
#pragma once

#include <QGraphicsSvgItem>
#include <QFont>
#include <QColor>

class PowerSupplyItem : public QGraphicsSvgItem {
public:
    explicit PowerSupplyItem(const QString& svgPath = QString(), QGraphicsItem* parent = nullptr);




    // State setters
    void setVoltage(double volts);
    void setCurrent(double amps);
    void setEnabled(bool on);
    void setLabel(const QString& name);          // Optional: human-readable label
    void setTextColor(const QColor& color);
    void setEnabledColor(const QColor& onColor, const QColor& offColor);
    void setFont(const QFont& font);
    void setPadding(qreal px);                   // Inner padding for text layout

    // State getters
    double voltage() const { return m_voltage; }
    double current() const { return m_current; }
    bool isEnabled() const { return m_enabled; }
    QString label() const { return m_label; }
    QColor textColor() const { return m_textColor; }
    QColor enabledColorOn() const { return m_enabledColorOn; }
    QColor enabledColorOff() const { return m_enabledColorOff; }
    QFont font() const { return m_font; }
    qreal padding() const { return m_padding; }

    // Load/replace SVG
    bool loadSvg(const QString& svgPath);

    // QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    // Data
    double  m_voltage   = 0.0;
    double  m_current   = 0.0;
    bool    m_enabled   = false;
    QString m_label     = QStringLiteral("PSU");

    // Style
    QFont   m_font              = QFont(QStringLiteral("Arial"), 20, QFont::Normal);
    QColor  m_textColor         = QColor(255, 0, 0);
    QColor  m_enabledColorOn    = QColor(0, 150, 0);
    QColor  m_enabledColorOff   = QColor(160, 0, 0);
    qreal   m_padding           = 8.0;

    // Cache
    QRectF  m_cachedBounds;

protected: void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
};
