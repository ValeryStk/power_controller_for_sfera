#ifndef MOCK_POWER_SERVER_H
#define MOCK_POWER_SERVER_H

#include <QTcpServer>
#include <QTcpSocket>

class MockPowerServer : public QTcpServer {
    Q_OBJECT
public:
    explicit MockPowerServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

#endif // MOCK_POWER_SERVER_H

