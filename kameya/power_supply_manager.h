#ifndef POWER_SUPPLY_MANAGER_H
#define POWER_SUPPLY_MANAGER_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>


struct PowerUnitParams{
    bool   isOn;
    double V;
    double I;
    double Ilim;
};

//!
//! \brief Предназначен для управления блоками питания
//!
class PowerSupplyManager: public QObject {
  Q_OBJECT

 public:
  PowerSupplyManager();
  ~PowerSupplyManager();
  QJsonObject get_power_states();
  QString getID();
  void checkPowersConection();
  bool getPowerStatus(const quint16 index);

  double getVoltage(const quint16 index);
  void setVoltage(const quint16 index,
                  double value);

  void setCurrentLimit(const quint16 index,
                       double value);
  double getCurrentLimit(const quint16 index);
  double getCurrentValue(const quint16 index);
  PowerUnitParams get_all_params_for_lamp_out(const quint16 index);


  void switchOnUnit(const quint16 index);
  void switchOffUnit(const quint16 unit);

  void switchOnAllUnits();
  void switchOffAllUnits();

  void increaseVoltageStepByStepToCurrentLimit(const quint16 index);
  void decreaseVoltageStepByStepToZero(const quint16 index);

  bool isPowerOutConnected(const int index);

 private:
  QTcpSocket*     m_socket;
  QHostAddress    m_hostAddress;
  QJsonObject     m_powers;

  void loadJsonConfig();
  double getValueFromMessage(const QString& msg);
  void getIpAndOutForIndex(const int index,
                           QString& ip,
                           int& out);
  int getPowerOutsSize() const;
  int maybeReconnectHost(const int index);
  void setInitialParams();

 private slots:
  void errorInSocket(QAbstractSocket::SocketError error);


};

#endif // POWER_SUPPLY_MANAGER_H
