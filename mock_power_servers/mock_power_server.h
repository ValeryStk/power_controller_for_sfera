/**
 * @file   mock_power_server.h
 * @author Valery Stanchyk
 * @date   2026-02-05
 * @brief  mock server for emulation power supply CPX400DP TCP/IP responses
 */

#ifndef MOCK_POWER_SERVER_H
#define MOCK_POWER_SERVER_H

#include <QTcpServer>
#include <QTcpSocket>

enum class MOCK_POWER_COMMADS {
    SET_OUT_STATE,
    GET_OUT_STATE,
    SET_V,
    GET_V,
    GET_I_lim,
    SET_I_lim,
    GET_I,
    SET_I
};

class MockPowerServer : public QTcpServer {
    Q_OBJECT
public:
    explicit MockPowerServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    struct powerParams {
        bool is_on_out_1 = true;
        bool is_on_out_2 = true;

        double I_limit_out_1 = 10.0;
        double I_limit_out_2 = 10.0;

        double Voltage_out_1 = 0.0;
        double Voltage_out_2 = 0.0;

        double I_current_out_1 = 0.0;
        double I_current_out_2 = 0.0;
    } m_params;
};

#endif  // MOCK_POWER_SERVER_H
