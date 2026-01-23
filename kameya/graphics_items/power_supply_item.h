// PowerSupplyItem.h
#pragma once

#include <QColor>
#include <QFont>
#include <QGraphicsSvgItem>

class PowerSupplyItem : public QGraphicsSvgItem {
public:
    explicit PowerSupplyItem(const QString& svgPath, const QString& name,
                             const QString& obj_name);
    ~PowerSupplyItem();

    // State setters
    void set_voltage_out_1(double volts);
    void set_current_out_1(double amps);
    void set_max_current_out_1(double amps);
    void set_enabled_out_1(bool on);
    void set_out_1_color(const QColor& color);
    void set_out_1_active();

    void set_voltage_out_2(double volts);
    void set_current_out_2(double amps);
    void set_max_current_out_2(double amps);
    void set_enabled_out_2(bool on);
    void set_out_2_color(const QColor& color);
    void set_out_2_active();

    void set_all_outs_unactive();
    void setLabel(const QString& name);

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
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

private:
    // Data
    double m_voltage_out_1 = 0.0;
    double m_current_out_1 = 0.0;
    double m_max_current_out_1 = 0.0;
    bool m_enabled_out_1 = false;
    bool m_is_out_1_active = false;
    QColor m_out_1_color;

    double m_voltage_out_2 = 0.0;
    double m_current_out_2 = 0.0;
    double m_max_current_out_2 = 0.0;
    bool m_enabled_out_2 = false;
    bool m_is_out_2_active = false;
    QColor m_out_2_color;

    QString m_label;
    QString m_object_name;

    // Style
    QFont m_font = QFont(QStringLiteral("Arial"), 20, QFont::Normal);
    QColor m_textColor = QColor(255, 0, 0);
    QColor m_enabledColorOn = QColor(0, 150, 0);
    QColor m_enabledColorOff = QColor(160, 0, 0);

    // Cache
    QRectF m_cachedBounds;
};
