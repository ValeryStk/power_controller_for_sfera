#ifndef TYPES_H
#define TYPES_H
#include <QMetaType>
#include <QString>
#include <QColor>
#include <QMap>
#include <QVector>


enum class LampsNumber{

    SIX_LAMPS = 6,
    FIVE_LAMPS = 5,
    FOUR_LAMPS = 4,
    THREE_LAMPS = 3,
    TWO_LAMPS = 2,
    ONE_LAMP = 1,
    NO_LAMPS = 0
};


struct PowerUnitParams{

    double V;
    double I;
    double Ilim;
    bool   isOn;
};

enum class PowerDevice:int{
    POWER_1 = 0,
    POWER_2 = 1,
    POWER_3 = 2,
    POWER_4 = 3
};


enum class DeviceParam{
    ID,
    IP,
    CONNECTION_STATE
};


struct PowerSupplies{

    QString id;
    QString ip;
    bool isConnected;

};

#endif // TYPES_H
