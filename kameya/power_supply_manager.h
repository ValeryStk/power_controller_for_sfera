#ifndef POWER_SUPPLY_MANAGER_H
#define POWER_SUPPLY_MANAGER_H

#include <QHostAddress>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTcpSocket>

struct PowerUnitParams {
    bool isOn;
    double V;
    double I;
    double Ilim;
};

//!
//! \brief Предназначен для управления блоками питания
//!
class PowerSupplyManager : public QObject {
    Q_OBJECT

public:
    PowerSupplyManager();
    ~PowerSupplyManager();
    QJsonObject get_power_states();
    QString getID();

    void checkPowersConection();

    bool getPowerStatus(const quint16 index, bool is_wait = false);

    double getVoltage(const quint16 index, bool is_wait = false);

    void setVoltage(const quint16 index, double value, bool is_wait = false);

    void setCurrentLimit(const quint16 index, double value,
                         bool is_wait = false);

    double getCurrentLimit(const quint16 index, bool is_wait = false);

    double getCurrentValue(const quint16 index, bool is_wait = false);

    double getVoltageProtectionValue(const quint16 index, bool is_wait = false);

    PowerUnitParams get_all_params_for_lamp_out(const quint16 index);

    void switchOnUnit(const quint16 index, bool is_wait = false);

    void switchOffUnit(const quint16 unit, bool is_whait = false);

    void switchOnAllUnits();
    void switchOffAllUnits();

    void increaseVoltageStepByStepToCurrentLimit(const quint16 index);
    void decreaseVoltageStepByStepToZero(const quint16 index);

    bool isPowerOutConnected(const int index);

private:
    QTcpSocket* m_socket;
    QHostAddress m_hostAddress;
    QJsonObject m_powers;

    void loadJsonConfig();
    double getValueFromMessage(const QString& msg);
    void getIpAndOutForIndex(const int index, QString& ip, int& out);
    int getPowerOutsSize() const;
    int maybeReconnectHost(const int index);
    void setInitialParams();

signals:
    void lamp_state_changed_to_ub(int lamp_index);
    void lamp_state_changed(int lamp_index, double voltage, double current);

    void power_state_changed(int power_index, int power_out, bool is_on);

private slots:
    void errorInSocket(QAbstractSocket::SocketError error);
};

#endif  // POWER_SUPPLY_MANAGER_H
