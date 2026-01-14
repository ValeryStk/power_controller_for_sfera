// PowerSupplyItem.h
#pragma once

#include <QGraphicsSvgItem>
#include <QFont>
#include <QColor>

class PowerSupplyItem : public QGraphicsSvgItem {
public:
    explicit PowerSupplyItem(const QString& svgPath, const QString& name);
    ~PowerSupplyItem();



    // State setters
    void set_voltage_out_1(double volts);
    void set_current_out_1(double amps);
    void set_enabled_out_1(bool on);

    void set_voltage_out_2(double volts);
    void set_current_out_2(double amps);
    void set_enabled_out_2(bool on);

    void setLabel(const QString& name);

    // Optional: human-readable label
    void setTextColor(const QColor& color);
    void setEnabledColor(const QColor& onColor, const QColor& offColor);
    void setFont(const QFont& font);
    void setPadding(qreal px);                   // Inner padding for text layout

    // State getters
    double voltage_out_1() const { return m_voltage_out_1; }
    double current_out_1() const { return m_current_out_1; }
    bool isEnabled_out_1() const { return m_enabled_out_1; }

    double voltage_out_2() const { return m_voltage_out_2; }
    double current_out_2() const { return m_current_out_2; }
    bool isEnabled_out_2() const { return m_enabled_out_2; }


    QString label() const { return m_label; }
    QColor textColor() const { return m_textColor; }
    QColor enabledColorOn() const { return m_enabledColorOn; }
    QColor enabledColorOff() const { return m_enabledColorOff; }
    QFont font() const { return m_font; }

    // Load/replace SVG
    bool loadSvg(const QString& svgPath);

    // QGraphicsItem
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    // Data
    double  m_voltage_out_1   = 0.0;
    double  m_current_out_1   = 0.0;
    bool    m_enabled_out_1   = false;

    double  m_voltage_out_2   = 0.0;
    double  m_current_out_2   = 0.0;
    bool    m_enabled_out_2   = false;


    QString m_label     = QStringLiteral("PSU");

    // Style
    QFont   m_font              = QFont(QStringLiteral("Arial"), 20, QFont::Normal);
    QColor  m_textColor         = QColor(255, 0, 0);
    QColor  m_enabledColorOn    = QColor(0, 150, 0);
    QColor  m_enabledColorOff   = QColor(160, 0, 0);

    // Cache
    QRectF  m_cachedBounds;

protected: void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
};
