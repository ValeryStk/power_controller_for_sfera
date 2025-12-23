#ifndef OPTICTABLE_H
#define OPTICTABLE_H
#include <QGraphicsItem>

struct AnglesScene {
    int leftSafety;
    int rightSafety;
    int bigSphere;
    int smallSphere;
    int linearLight;
};

class OpticTable:public QGraphicsItem
{
public:
    OpticTable();

public:
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setBulbOff(int bi);
    void setAngle(int newAngle);
    void setAngles(AnglesScene &newAngles);

private:
    int angle;
    AnglesScene m_angles;
    QVector<bool> isBulbsOn;
    void drawLamps(QPainter *painter);

};

#endif // OPTICTABLE_H
