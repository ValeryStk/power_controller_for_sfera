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
    checkPowersConection();
    setInitialParams();
    switchOnAllUnits();



}

PowerSupplyManager::~PowerSupplyManager() {
    qDebug()<<"power manager destructor.......";
    switchOffAllUnits();
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();
}

QJsonObject PowerSupplyManager::get_power_states()
{
    return m_powers;
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
    //qDebug() << "voltage: " << response;
    return getValueFromMessage(response);
}

void PowerSupplyManager::setVoltage(const quint16 index,
                                    double value) {
    //qDebug() << "set voltage: " << value;
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

PowerUnitParams PowerSupplyManager::get_all_params_for_lamp_out(const quint16 index)
{

    return {getPowerStatus(index),
            getVoltage(index),
            getCurrentValue(index),
            getCurrentLimit(index)};
}

bool PowerSupplyManager::getPowerStatus(const quint16 index) {
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
    double target_current = m_powers["lamps"].toArray()[index].toObject().value("max_current").toDouble();

    double currentValue = getCurrentValue(index);
    int dolbo_counter = 0;
    while(currentValue < target_current){
        if(m_socket->state() != QTcpSocket::ConnectedState){
            break;
        }
        double voltage = getVoltage(index);
        currentValue = getCurrentValue(index);
        if((target_current-currentValue)<=0.005){
            ++dolbo_counter;
        }
        voltage = qAbs(voltage + 0.1);
        setVoltage(index,voltage);
        Sleep(300);
        if(dolbo_counter == 10){
            //qDebug()<<"DOLBO COUNTER EXIT--->"<<"current value --> "<<currentValue<<" --tc-- "<<target_current;
            //qDebug()<<"***************************************************************************************";
            break;
        }
    }
}

void PowerSupplyManager::decreaseVoltageStepByStepToZero(const quint16 index)
{
    maybeReconnectHost(index);
    while(true){
        if(m_socket->state() != QTcpSocket::ConnectedState){
            break;
        }
        double voltage = getVoltage(index);
        voltage = voltage - 0.1;
        if(voltage<0)voltage = 0;
        setVoltage(index,voltage);
        Sleep(300);
        if(voltage<=0)break;
    }
}

bool PowerSupplyManager::isPowerOutConnected(const int index) {
    QJsonArray lamps = m_powers["lamps"].toArray();
    return lamps[index].toObject()["conection_state"].toBool();
}

void PowerSupplyManager::errorInSocket(QAbstractSocket::SocketError error) {
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::HostNotFoundError:
    case QAbstractSocket::SocketAccessError:
    case QAbstractSocket::SocketResourceError:
    case QAbstractSocket::SocketTimeoutError:
    case QAbstractSocket::DatagramTooLargeError:
    case QAbstractSocket::NetworkError:
    case QAbstractSocket::AddressInUseError:
    case QAbstractSocket::SocketAddressNotAvailableError:
    case QAbstractSocket::UnsupportedSocketOperationError:
    case QAbstractSocket::UnfinishedSocketOperationError:
    case QAbstractSocket::ProxyAuthenticationRequiredError:
    case QAbstractSocket::SslHandshakeFailedError:
    case QAbstractSocket::ProxyConnectionRefusedError:
    case QAbstractSocket::ProxyConnectionClosedError:
    case QAbstractSocket::ProxyConnectionTimeoutError:
    case QAbstractSocket::ProxyNotFoundError:
    case QAbstractSocket::ProxyProtocolError:
    case QAbstractSocket::OperationError:
    case QAbstractSocket::SslInternalError:
    case QAbstractSocket::SslInvalidUserDataError:
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::UnknownSocketError:
        qDebug() << "net error code: "<<error;
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
    if (current_host == new_host && m_socket->state() == QTcpSocket::ConnectedState)
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
        if(state == QTcpSocket::ConnectedState) {
            object["conection_state"] = true;
            object["power_out_state"] = getPowerStatus(i);
        }else {
            object["conection_state"] = false;
            object["power_out_state"] = false;
        }
        lamps[i] = object;
    };
    m_powers["lamps"] = lamps;
}

void PowerSupplyManager::setInitialParams() {

    for (int i = 0; i < getPowerOutsSize(); ++i) {
        if (isPowerOutConnected(i)) {
            double max_current = m_powers["lamps"].toArray()[i].toObject().value("max_current").toDouble();
            setCurrentLimit(i, max_current);
            Sleep(500);
        }
    }
}
