#include "power_supply_manager.h"

#include <QtGlobal>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <synchapi.h>

#include "commands_builder.h"
#include "config.h"
#include "json_utils.h"
#include "text_log_constants.h"

constexpr int host_port = 9221;

namespace {
void wait() { Sleep(WAIT_INTERVAL); }
}  // end namespace

PowerSupplyManager::PowerSupplyManager() {
    qInfo() << tlc::kPowerManagerConstructor;
}

PowerSupplyManager::~PowerSupplyManager() {
    qInfo() << tlc::kPowerManagerDestructor;
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();
    qInfo() << tlc::kEndOfTheLog;
}

QJsonObject PowerSupplyManager::get_power_states() { return m_powers; }

QString PowerSupplyManager::getID() {
    m_socket->write(pwr::makeGetDeviceID_Command());
    m_socket->waitForReadyRead();
    return QString::fromLocal8Bit(m_socket->readAll());
}

void PowerSupplyManager::loadJsonConfig() {
    qInfo() << "Load lamps.json config";
    bool is_json_valid =
        jsn::getJsonObjectFromFile(global::config_json_file_name, m_powers);
    if (!is_json_valid) {
        qCritical() << "json config is not loaded";
        return;
    }
    auto lamps = m_powers[global::kJsonKeyLampsArray].toArray();
    for (int i = 0; i < lamps.size(); ++i) {
        qInfo() << QString("lamp %1: current limit %2 A")
                       .arg(i + 1)
                       .arg(lamps[i].toObject()["max_current"].toDouble());
    }
}

double PowerSupplyManager::getVoltage(const quint16 index, bool is_wait) {
    if (!isPowerOutConnected(index)) return 0;
    int out = maybeReconnectHost(index);
    m_socket->write(pwr::makeGetVoltageValueCommand(out));
    m_socket->waitForReadyRead();
    QString response = QString::fromLocal8Bit(m_socket->readAll());
    if (is_wait) wait();
    return getValueFromMessage(response);
}

void PowerSupplyManager::setVoltage(const quint16 index, double value,
                                    bool is_wait) {
    if (!isPowerOutConnected(index)) return;
    int out = maybeReconnectHost(index);
    m_socket->write(pwr::makeSetVcommand(out, value));
    m_socket->waitForBytesWritten();
    if (is_wait) wait();
}

void PowerSupplyManager::setCurrentLimit(const quint16 index, double value,
                                         bool is_wait) {
    if (!isPowerOutConnected(index)) return;
    int out = maybeReconnectHost(index);
    m_socket->write(pwr::makeSetCurrentLimitCommand(out, value));
    m_socket->waitForBytesWritten();
    if (is_wait) wait();
}

double PowerSupplyManager::getCurrentLimit(const quint16 index, bool is_wait) {
    if (!isPowerOutConnected(index)) return 0;
    int out = maybeReconnectHost(index);
    m_socket->write(pwr::makeGetCurrentLimitCommand(out));
    m_socket->waitForReadyRead();
    auto message = QString::fromLocal8Bit(m_socket->readAll());
    double value = getValueFromMessage(message);
    if (is_wait) wait();
    return value;
}

double PowerSupplyManager::getCurrentValue(const quint16 index, bool is_wait) {
    if (!isPowerOutConnected(index)) return 0;
    int out = maybeReconnectHost(index);
    m_socket->write(pwr::makeGetCurrentValueCommand(out));
    m_socket->waitForReadyRead();
    double value =
        getValueFromMessage(QString::fromLocal8Bit(m_socket->readAll()));
    if (is_wait) wait();
    return value;
}

double PowerSupplyManager::getVoltageProtectionValue(const quint16 index,
                                                     bool is_wait) {
    if (!isPowerOutConnected(index)) return 0;
    int out = maybeReconnectHost(index);
    m_socket->write(pwr::makeGetProtectionVoltageValueCommand(out));
    m_socket->waitForReadyRead();
    QString response = QString::fromLocal8Bit(m_socket->readAll());
    response.remove('\r').remove('\n');
    if (is_wait) wait();
    return response.toDouble();
}

PowerUnitParams PowerSupplyManager::get_all_params_for_lamp_out(
    const quint16 index) {
    PowerUnitParams params;
    params.isOn = getPowerStatus(index, true);
    wait();
    params.V = getVoltage(index, true);
    wait();
    params.I = getCurrentValue(index, true);
    params.Ilim = getCurrentLimit(index, true);
    return params;
}

bool PowerSupplyManager::getPowerStatus(const quint16 index, bool is_wait) {
    int out = maybeReconnectHost(index);
    m_socket->write(pwr::makeGetSwitchStateCommand(out));
    m_socket->waitForReadyRead();
    QString temp = QString::fromLocal8Bit(m_socket->readAll());
    temp.remove('\r').remove('\n');
    auto result = (bool)temp.toInt();
    if (is_wait) wait();
    return result;
}

void PowerSupplyManager::switchOnUnit(const quint16 index, bool is_wait) {
    if (!isPowerOutConnected(index)) return;
    int out = maybeReconnectHost(index);
    m_socket->write(pwr::makeSwitchOnUnitCommand(out));
    m_socket->waitForBytesWritten();
    m_socket->readAll();
    if (is_wait) wait();
}

void PowerSupplyManager::switchOffUnit(const quint16 index, bool is_whait) {
    if (!isPowerOutConnected(index)) return;
    int out = maybeReconnectHost(index);
    m_socket->write(pwr::makeSwitchOffUnitCommand(out));
    m_socket->waitForBytesWritten();
    if (is_whait) wait();
}

void PowerSupplyManager::switchOnAllUnits() {
    for (int i = 0; i < getPowerOutsSize(); ++i) {
        switchOnUnit(i, true);
        wait();
    }
}

void PowerSupplyManager::switchOffAllUnits() {
    for (int i = 0; i < getPowerOutsSize(); ++i) {
        switchOffUnit(i);
    };
}

void PowerSupplyManager::increaseVoltageStepByStepToCurrentLimit(
    const quint16 index) {
    maybeReconnectHost(index);
    double target_current = m_powers[global::kJsonKeyLampsArray]
                                .toArray()[index]
                                .toObject()
                                .value(global::kJsonKeyMaxCurrent)
                                .toDouble();
    int fail_counter = 0;
    double last_current_value = 0.0;
    double last_voltage_value = 0.0;

    while (last_current_value < target_current) {
        ++fail_counter;

        if (m_socket->state() != QTcpSocket::ConnectedState) {
            qWarning() << QString(tlc::kFailIncreasingProcessSocketUnconnected)
                              .arg(index + 1);
            emit lamp_state_changed_to_ub(index, last_voltage_value,
                                          last_current_value, false);
            return;
        }
        last_current_value = getCurrentValue(index, true);
        last_voltage_value = getVoltage(index, true);
        emit update_power_out(index, last_voltage_value, last_current_value);
        if (last_voltage_value < 0) last_voltage_value = 0;

        if ((target_current - last_current_value) <=
            global::kCurrentTargetAccuracy) {
            setVoltage(index, MAX_VOLTAGE);
            qDebug() << "LAMP " << index + 1
                     << "SET MAX VOLTAGE: " << MAX_VOLTAGE;
            for (int i = 0; i < TRY_AGAIN_COUNTER; ++i) {
                setVoltage(index, MAX_VOLTAGE, true);
            }
            last_voltage_value = getVoltage(index, true);
            last_current_value = getCurrentValue(index, true);
            emit lamp_state_changed(index, last_voltage_value,
                                    last_current_value);
            break;
        }

        double new_voltage_value = last_voltage_value + VOLTAGE_INCREASE_STEP;
        setVoltage(index, new_voltage_value, true);
        last_voltage_value = getVoltage(index, true);
        if (fail_counter == 10 && last_voltage_value <= VOLTAGE_INCREASE_STEP) {
            QString warning =
                "POWER %1 IS ON BUT VOLTAGE (%2) IS LESS THAN VOLTAGE INCREASE "
                "STEP (%3)";
            QString message = warning.arg(global::get_power_num_by_index(index))
                                  .arg(getVoltage(index, true))
                                  .arg(VOLTAGE_INCREASE_STEP);
            qWarning() << message;
            emit lamp_state_changed_to_ub(index, last_voltage_value,
                                          last_current_value, true);
            return;
        };
    }
}

void PowerSupplyManager::decreaseVoltageStepByStepToZero(const quint16 index) {
    maybeReconnectHost(index);
    auto power_num = global::get_power_num_by_index(index);
    auto out_num = global::get_power_out_by_index(index);
    int fail_counter = 0;
    double start_voltage = getVoltage(index, true);
    while (getVoltage(index, true) > global::kVoltageZeroAccuracy) {
        ++fail_counter;
        if (m_socket->state() != QTcpSocket::ConnectedState) {
            qWarning() << QString(tlc::kFailDecreasingProcessSocketUnconnected)
                              .arg(index + 1);
            emit lamp_state_changed_to_ub(index, 0, 0, false);
            break;
        }

        double voltage = getVoltage(index, true);
        voltage = voltage - VOLTAGE_DECREASE_STEP;
        if (voltage < 0) voltage = 0;

        setVoltage(index, voltage, true);
        voltage = getVoltage(index, true);
        double current = getCurrentValue(index, true);
        emit update_power_out(index, voltage, current);
        if (voltage <= global::kVoltageZeroAccuracy) {
            setVoltage(index, 0, true);
            emit lamp_state_changed(index, voltage, getCurrentValue(false));
            break;
        }
        if (fail_counter == 3 &&
            ((start_voltage - voltage) <= VOLTAGE_DECREASE_STEP)) {
            QString message =
                "POWER %1 OUT %2 IS ON BUT VOLTAGE IS NOT POSSIBLE TO DECREASE";
            qWarning() << message.arg(power_num).arg(out_num);
            emit lamp_state_changed_to_ub(index, voltage, 0, true);
            break;
        };
    }
}

bool PowerSupplyManager::isPowerOutConnected(const int index) {
    QJsonArray lamps = m_powers[global::kJsonKeyLampsArray].toArray();
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
            qDebug() << "net error code: " << error;
            break;
    }
}

void PowerSupplyManager::switch_on_one_lamp(const int index) {
    qDebug() << "**** CHECK SWITCH ON ONE LAMP ROUTE ****";
    increaseVoltageStepByStepToCurrentLimit(index);
}

void PowerSupplyManager::switch_off_one_lamp(const int index) {
    qDebug() << "**** CHECK SWITCH OFF ONE LAMP ROUTE ****";
    decreaseVoltageStepByStepToZero(index);
}

void PowerSupplyManager::switch_on_all_lamps() {
    qDebug() << "ALL LAMPS ON PROCESS STARTED..........";
    for (int i = 0; i < NUMBER_OF_LAMPS; ++i) {
        increaseVoltageStepByStepToCurrentLimit(i);
    };
}

void PowerSupplyManager::switch_off_all_lamps() {
    qDebug() << "ALL LAMPS OFF PROCESS STARTED..........";
    for (int i = MAX_CURRENT_LAMP_INDEX; i >= 0; --i) {
        decreaseVoltageStepByStepToZero(i);
    };
}

void PowerSupplyManager::test_all_powers() {
    QVector<PowerUnitParams> test_result(NUMBER_OF_LAMPS);
    for (int i = 0; i < NUMBER_OF_LAMPS; ++i) {
        test_result[i] = get_all_params_for_lamp_out(i);
        wait();
    }
    emit test_finished(test_result);
}

void PowerSupplyManager::initSocket() {
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            SLOT(errorInSocket(QAbstractSocket::SocketError)),
            Qt::DirectConnection);
    connect(this, SIGNAL(make_one_lamp_off(int)), this,
            SLOT(switch_off_one_lamp(int)), Qt::DirectConnection);
    connect(this, SIGNAL(make_one_lamp_on(int)), this,
            SLOT(switch_on_one_lamp(int)), Qt::DirectConnection);
    connect(this, SIGNAL(make_all_lamps_off()), this,
            SLOT(switch_off_all_lamps()), Qt::DirectConnection);
    connect(this, SIGNAL(make_all_lamps_on()), this,
            SLOT(switch_on_all_lamps()), Qt::DirectConnection);
    // test_all
    connect(this, SIGNAL(test_all()), this, SLOT(test_all_powers()),
            Qt::DirectConnection);
    qDebug() << "SOCKET IN NEW CONTEXT**************************************";
    loadJsonConfig();
    checkPowersConection();
    wait();
    setInitialParams();
    wait();
    switchOnAllUnits();
    wait();
    test_all_powers();
}

void PowerSupplyManager::getIpAndOutForIndex(const int index, QString& ip,
                                             int& out) {
    QJsonArray lamps = m_powers[global::kJsonKeyLampsArray].toArray();
    ip = lamps[index].toObject()["ip"].toString();
    out = lamps[index].toObject()["out"].toInt();
}

int PowerSupplyManager::getPowerOutsSize() const {
    return m_powers[global::kJsonKeyLampsArray].toArray().size();
}

int PowerSupplyManager::maybeReconnectHost(const int index) {
    int out;
    QString new_host;
    getIpAndOutForIndex(index, new_host, out);
    QString current_host = m_hostAddress.toString();
    if (current_host == new_host &&
        m_socket->state() == QTcpSocket::ConnectedState)
        return out;
    m_socket->disconnectFromHost();
    m_hostAddress.setAddress(new_host);
    m_socket->connectToHost(m_hostAddress, host_port, QIODevice::ReadWrite);
    m_socket->waitForConnected(1000);
    return out;
}

void PowerSupplyManager::checkPowersConection() {
    QJsonArray lamps = m_powers[global::kJsonKeyLampsArray].toArray();
    for (int i = 0; i < getPowerOutsSize(); ++i) {
        auto object = lamps[i].toObject();
        maybeReconnectHost(i);
        auto state = m_socket->state();
        if (state == QTcpSocket::ConnectedState) {
            object["conection_state"] = true;
            object["power_out_state"] = getPowerStatus(i);
        } else {
            object["conection_state"] = false;
            object["power_out_state"] = false;
        }
        lamps[i] = object;
    };
    m_powers[global::kJsonKeyLampsArray] = lamps;
}

void PowerSupplyManager::setInitialParams() {
    for (int i = 0; i < getPowerOutsSize(); ++i) {
        if (isPowerOutConnected(i)) {
            double max_current = m_powers[global::kJsonKeyLampsArray]
                                     .toArray()[i]
                                     .toObject()
                                     .value("max_current")
                                     .toDouble();
            setCurrentLimit(i, max_current);
            wait();
        }
    }
}
