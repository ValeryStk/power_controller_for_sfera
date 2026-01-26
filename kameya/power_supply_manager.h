#ifndef POWER_SUPPLY_MANAGER_H
#define POWER_SUPPLY_MANAGER_H

#include <QHostAddress>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTcpSocket>

struct PowerUnitParams {
    bool isOn = false;
    double V = 0.0;
    double I = 0.0;
    double Ilim = 0.0;
    int lamp_index = 0;
};

//!
//! \brief Предназначен для управления блоками питания
//!
class PowerSupplyManager : public QObject {
    Q_OBJECT
    friend class MockPowerServer;

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

    static double getValueFromMessage(const QString& msg) {
        static const QStringList prefixes = {"V1", "V2", "A", "V", "I1", "I2"};

        QString temp = msg;

        for (const auto& prefix : prefixes) {
            if (temp.startsWith(prefix)) {
                temp.remove('\r').remove('\n').remove(' ').remove(prefix);
                bool isOk = false;
                double result = temp.toDouble(&isOk);
                return result;
            }
        }

        return 0.0;
    };

private:
    QTcpSocket* m_socket;

    QHostAddress m_hostAddress;
    QJsonObject m_powers;

    void loadJsonConfig();
    void getIpAndOutForIndex(const int index, QString& ip, int& out);
    int getPowerOutsSize() const;
    int maybeReconnectHost(const int index);
    void setInitialParams();

signals:
    // commands signals
    void test_all();
    void make_one_lamp_on(int);
    void make_one_lamp_off(int);
    void make_all_lamps_on();
    void make_all_lamps_off();

    // states changed signals
    void states_changed(QVector<PowerUnitParams>);
    void test_finished(QVector<PowerUnitParams>);
    void lamp_state_changed_to_ub(int lamp_index, double voltage,
                                  double current, bool is_on);
    void lamp_state_changed(int lamp_index, double voltage, double current);
    void power_state_changed(int power_index, int power_out, bool is_on);
    void socketInitialized();

private slots:
    void errorInSocket(QAbstractSocket::SocketError error);
    void switch_on_one_lamp(const int index);
    void switch_off_one_lamp(const int index);
    void switch_on_all_lamps();
    void switch_off_all_lamps();
    void test_all_powers();
public slots:
    void initSocket();
};

#endif  // POWER_SUPPLY_MANAGER_H
