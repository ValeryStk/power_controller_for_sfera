#ifndef TYPES_H
#define TYPES_H
#include <QMetaType>
#include <QString>
#include <QColor>
#include <QMap>
#include <QVector>


enum class CalibrSteps{

    SET_INITIAL_PARAMS,
    GET_OPTIMAL_EXPO,
    GATHERING_DATA,
    AFTER_SWITCH_OFF_ONE_LAMP
};

enum class SpectralGetting{

    CloseShutter,
    CapturingBlacks,
    OpenShutter,
    CapturingReal,
    GoToNextWave

};

enum class LampsNumber{

    SIX_LAMPS = 6,
    FIVE_LAMPS = 5,
    FOUR_LAMPS = 4,
    THREE_LAMPS = 3,
    TWO_LAMPS = 2,
    ONE_LAMP = 1,
    NO_LAMPS = 0
};

enum class ChangeAngleSource{

    JUST_UPdate = 0,
    BIG_SPHERE = 1,
    SMALL_SPHERE = 2,
    LINEAR_LIGHTS = 3

};

enum SensorType{

    VISIBLE_RANGE_SENSOR,
    INFRA_RED_SENSOR,
    UKNOWN_SENSOR
};
#pragma once
Q_DECLARE_METATYPE(SensorType)

enum class PowerOuts{

    POW1_OUT1,
    POW1_OUT2,
    POW2_OUT1,
    POW2_OUT2,
    POW3_OUT1,
    POW3_OUT2,
    POW4_OUT1,
    POW4_OUT2,
    UKNOWN
};
#pragma once
Q_DECLARE_METATYPE(PowerOuts)

enum class Param{
    V,
    I,
    Ilim,
    isOn
};
#pragma once
Q_DECLARE_METATYPE(Param)

enum LightSource{

    BIG_SPHERE,
    SMALL_SPHERE,
    LINEAR_LIGHTS,
    NOT_POINTED
};

struct logicCalibr{

    CalibrSteps calibrAction;
    LampsNumber numberOfLamps;
    SensorType  sensorType;
    LightSource lightSource;

};

struct solarTasksResults{

  bool gradeSetted;
  bool slitSetted;
  bool shutterOpened;
  bool waveLengthSetted;
  bool filterSetted;

};

struct source_expositions{

    double big_sphere;
    double small_spheare;
    double linear_lights;
    bool   isAllExpoGood;

};


enum SolarTaskReportType{

    SOLAR_TASK_SHUTTER,
    SOLAR_TASK_GRATING,
    SOLAR_TASK_WAVELENGTH,
    SOLAR_TASK_FILTER,
    SOLAR_TASK_SLIT

};
#pragma once
Q_DECLARE_METATYPE(SolarTaskReportType)

struct GratingParams{

    int gratingIndex;
    int grooves;
    double minWaveLength;
    double maxWaveLength;
    double blazeAngle;

};
#pragma once
Q_DECLARE_METATYPE(GratingParams)

struct FilterParams{

    int filterId;
    QString filterName;
    int minWaveLength;
    int maxWaveLength;
    QString metaName;
};
#pragma once
Q_DECLARE_METATYPE(FilterParams)


struct ShutterText{
    const QString sOpened  = "Затвор открыт";
    const QString sClosed  = "Затвор закрыт";
    const QString sUnknown = "Затвор не определён";
};

struct SourcesNames{
    const QString bigSphere = "BigSphere";
    const QString smallSphere = "SmallSphere";
    const QString linearLights = "LinearLights";
};

struct SensorNames{
    const QString irSensor = "IR_sensor";
    const QString visibleSensor = "Visible_sensor";
};

struct TestResults {

    bool powerSupply_1;
    bool powerSupply_2;
    bool powerSupply_3;
    bool rotateTable;
    bool solarDevice;
    bool spectralVisibleDevice;
    bool spectralInfraredDevice;

};

struct visibleSensorParams{
    int grade;
    int slit;
    QVector <int> lambdas;
    int currentWaveIndex;
    int numberOfLampsOn;
};

struct infraRedSensorParams{
    int grade;
    int slit;
    QVector <int> lambdas;
    int currentWaveIndex;
    int numberOfLampsOn;
};


struct calibrParams{

    visibleSensorParams visibleSensor;
    infraRedSensorParams infraRedSensor;

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
#pragma once
Q_DECLARE_METATYPE(PowerDevice)

enum class DeviceParam{
    ID,
    IP,
    CONNECTION_STATE
};
#pragma once
Q_DECLARE_METATYPE(DeviceParam)

struct PowerSupplies{

    QString id;
    QString ip;
    bool isConnected;

};

#endif // TYPES_H
