#include "power_supply_manager.h"

#include <QtGlobal>
#include "json_utils.h"
#include "commands_builder.h"
#include "Windows.h"

constexpr int host_port = 9221;

PowerSupplyManager::PowerSupplyManager() {
  m_socket = new QTcpSocket;
  connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
          SLOT(errorInSocket(QAbstractSocket::SocketError)));
  loadJsonConfig();
  switchOnAllUnits();
  checkPowersConection();
  setInitialParams();

}

PowerSupplyManager::~PowerSupplyManager() {
  switchOffAllUnits();
  if (m_socket->state() == QAbstractSocket::ConnectedState)
    m_socket->disconnectFromHost();
}

QString PowerSupplyManager::getID() {
  m_socket->write(pwr::makeGetDeviceID_Command());
  m_socket->waitForReadyRead();
  return QString::fromLocal8Bit(m_socket->readAll());
}

void PowerSupplyManager::loadJsonConfig() {
  jsn::getJsonObjectFromFile("ir_lamps.json", m_powers);
}

double PowerSupplyManager::getVoltage(const quint16 index) {
  if (!isPowerOutConnected(index))
    return 0;
  int out = maybeReconnectHost(index);
  m_socket->write(pwr::makeGetVoltageValueCommand(out));
  m_socket->waitForReadyRead();
  QString response = QString::fromLocal8Bit(m_socket->readAll());
  qDebug() << "voltage: " << response;
  return getValueFromMessage(response);
}

void PowerSupplyManager::setVoltage(const quint16 index,
                                    double value) {
  qDebug() << "set voltage: " << value;
  if (!isPowerOutConnected(index))
    return;
  int out = maybeReconnectHost(index);
  m_socket->write(pwr::makeSetVcommand(out, value));
  m_socket->waitForBytesWritten();
}

void PowerSupplyManager::setCurrentLimit(const quint16 index,
                                         double value) {
  if (!isPowerOutConnected(index))
    return;
  int out = maybeReconnectHost(index);
  m_socket->write(pwr::makeSetCurrentLimitCommand(out, value));
  m_socket->waitForBytesWritten();
}

double PowerSupplyManager::getCurrentLimit(const quint16 index) {
  if (!isPowerOutConnected(index))
    return 0;
  int out = maybeReconnectHost(index);
  m_socket->write(pwr::makeGetCurrentLimitCommand(out));
  m_socket->waitForReadyRead();
  return getValueFromMessage(QString::fromLocal8Bit(m_socket->readAll()));
}

double PowerSupplyManager::getCurrentValue(const quint16 index) {
  if (!isPowerOutConnected(index))
    return 0;
  int out = maybeReconnectHost(index);
  m_socket->write(pwr::makeGetCurrentValueCommand(out));
  m_socket->waitForReadyRead();
  return getValueFromMessage(QString::fromLocal8Bit(m_socket->readAll()));
}

bool PowerSupplyManager::getPowerStatus(const quint16 index) {
  if (!isPowerOutConnected(index))
    return false;
  int out = maybeReconnectHost(index);
  m_socket->write(pwr::makeGetSwitchStateCommand(out));
  m_socket->waitForReadyRead();
  QString temp = QString::fromLocal8Bit(m_socket->readAll());
  temp.remove('\r').remove('\n');
  auto result = (bool)temp.toInt();
  return result;
}

void PowerSupplyManager::switchOnUnit(const quint16 index) {
  if (!isPowerOutConnected(index))
    return;
  int out = maybeReconnectHost(index);
  m_socket->write(pwr::makeSwitchOnUnitCommand(out));
  m_socket->waitForBytesWritten();
}

void PowerSupplyManager::switchOffUnit(const quint16 index) {
  if (!isPowerOutConnected(index))
    return;
  int out = maybeReconnectHost(index);
  m_socket->write(pwr::makeSwitchOffUnitCommand(out));
  m_socket->waitForBytesWritten();
}

void PowerSupplyManager::switchOnAllUnits() {
  for (int i = 0; i < getPowerOutsSize(); ++i) {
    switchOnUnit(i);
  }
  checkPowersConection();
}

void PowerSupplyManager::switchOffAllUnits() {
  for (int i = 0; i < getPowerOutsSize(); ++i) {
    switchOffUnit(i);
  };
  checkPowersConection();
}

void PowerSupplyManager::increaseVoltageStepByStepToCurrentLimit(const quint16 index)
{
    maybeReconnectHost(index);
    setCurrentLimit(index,10);
    double current = getCurrentValue(index);
    qDebug()<<"current --> "<<current;
    double currentLimit = getCurrentLimit(index);
    qDebug()<<"current limit --> "<<currentLimit;
    for(int i=0;i<10;++i){
    double voltage = getVoltage(index);
    //qDebug()<<"voltage --> "<<voltage;
    voltage = qAbs(voltage+0.5);
    setVoltage(index,voltage);
    Sleep(300);
    qDebug()<<i<<"------------------ iteration ---------------------------";
    }
}

void PowerSupplyManager::decreaseVoltageStepByStepToZero(const quint16 index)
{
    maybeReconnectHost(index);
    setCurrentLimit(index,10);
    double current = getCurrentValue(index);
    qDebug()<<"current --> "<<current;
    double currentLimit = getCurrentLimit(index);
    qDebug()<<"current limit --> "<<currentLimit;

    while(true){
    double voltage = getVoltage(index);
    qDebug()<<"voltage --> "<<voltage;
    voltage = voltage - 0.5;
    setVoltage(index,voltage);
    Sleep(300);
    if(voltage<0)break;
    }
}

bool PowerSupplyManager::isPowerOutConnected(const int index) {
  QJsonArray lamps = m_powers["lamps"].toArray();
  return lamps[index].toObject()["conection_state"].toBool();
}

void PowerSupplyManager::errorInSocket(QAbstractSocket::SocketError error) {
  qDebug() << error;
  switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
      break;
    case QAbstractSocket::RemoteHostClosedError:
      break;
    case QAbstractSocket::HostNotFoundError:
      break;
    case QAbstractSocket::SocketAccessError:
      break;
    case QAbstractSocket::SocketResourceError:
      break;
    case QAbstractSocket::SocketTimeoutError:
      break;
    case QAbstractSocket::DatagramTooLargeError:
      break;
    case QAbstractSocket::NetworkError:
      qDebug() << "No connection error.";
      break;
    case QAbstractSocket::AddressInUseError:
      break;
    case QAbstractSocket::SocketAddressNotAvailableError:
      break;
    case QAbstractSocket::UnsupportedSocketOperationError:
      break;
    case QAbstractSocket::UnfinishedSocketOperationError:
      break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
      break;
    case QAbstractSocket::SslHandshakeFailedError:
      break;
    case QAbstractSocket::ProxyConnectionRefusedError:
      break;
    case QAbstractSocket::ProxyConnectionClosedError:
      break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
      break;
    case QAbstractSocket::ProxyNotFoundError:
      break;
    case QAbstractSocket::ProxyProtocolError:
      break;
    case QAbstractSocket::OperationError:
      break;
    case QAbstractSocket::SslInternalError:
      break;
    case QAbstractSocket::SslInvalidUserDataError:
      break;
    case QAbstractSocket::TemporaryError:
      break;
    case QAbstractSocket::UnknownSocketError:
      break;
  }
}

double PowerSupplyManager::getValueFromMessage(const QString& msg) {
  //qDebug()<<"power_msg: "<<msg;
  if (msg.contains("V1")) {
    QString temp = msg;
    temp.remove('\r').remove('\n').replace("V1 ", "");
    return temp.toDouble();
  } else if (msg.contains("V2")) {
    QString temp = msg;
    temp.remove('\r').remove('\n').replace("V2 ", "");
    return temp.toDouble();
  } else if (msg.contains("A")) {
    QString temp = msg;
    temp.remove('\r').remove('\n').replace("A", "");
    return temp.toDouble();
  } else if (msg.contains("V")) {
    QString temp = msg;
    temp.remove('\r').remove('\n').replace("V", "");
    return temp.toDouble();
  }
  return 0;
}

void PowerSupplyManager::getIpAndOutForIndex(const int index,
                                             QString& ip,
                                             int& out) {

  QJsonArray lamps = m_powers["lamps"].toArray();
  ip = lamps[index].toObject()["ip"].toString();
  out = lamps[index].toObject()["out"].toInt();
}

int PowerSupplyManager::getPowerOutsSize() const {
  return m_powers["lamps"].toArray().size();
}

int PowerSupplyManager::maybeReconnectHost(const int index) {
  int out;
  QString new_host;
  getIpAndOutForIndex(index, new_host, out);
  QString current_host = m_hostAddress.toString();
  if (current_host == new_host)
    return out;
  m_socket->disconnectFromHost();
  m_hostAddress.setAddress(new_host);
  m_socket->connectToHost(m_hostAddress, host_port, QIODevice::ReadWrite);
  m_socket->waitForConnected(1000);
  return out;
}

void PowerSupplyManager::checkPowersConection() {
  QJsonArray lamps = m_powers["lamps"].toArray();
  for (int i = 0; i < getPowerOutsSize(); ++i) {
    auto object = lamps[i].toObject();
    maybeReconnectHost(i);
    auto state = m_socket->state();
    switch (state) {
      case QTcpSocket::UnconnectedState:
        object["conection_state"] = false;
        object["power_out_state"] = false;
        break;
      case QTcpSocket::ConnectedState:
        object["conection_state"] = true;
        object["power_out_state"] = getPowerStatus(i);
        break;
    }
    lamps[i] = object;
  };
  m_powers["lamps"] = lamps;
  qDebug() << m_powers;
}

void PowerSupplyManager::setInitialParams() {


  for (int i = 0; i < getPowerOutsSize(); ++i) {
    if (isPowerOutConnected(i)) {
      setCurrentLimit(i, 0);
      setVoltage(i, m_powers["voltage"].toDouble());
    }
  }
}
