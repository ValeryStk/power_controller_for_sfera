#ifndef PROGRESS_SPINNER_ITEM_H
#define PROGRESS_SPINNER_ITEM_H

#include <QGraphicsItem>
#include <QPainter>

class SpinnerItem : public QGraphicsItem {
public:
    explicit SpinnerItem(QGraphicsItem *parent = nullptr);
    ~SpinnerItem() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    void rotate();  // запуск анимации
    void stop();    // остановка и сброс

protected:
    void advance(int step) override;

private:
    qreal m_outerRotation;  // угол вращения внешнего круга
    qreal m_innerRotation;  // угол вращения внутренней дуги
    qreal m_opacity;        // прозрачность дуги
    bool m_fadeOut;  // направление изменения прозрачности
};

#endif  // PROGRESS_SPINNER_ITEM_H
