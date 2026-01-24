#include "mock_power_server.h"

#include <QDebug>

MockPowerServer::MockPowerServer(QObject *parent) : QTcpServer(parent) {}

void MockPowerServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    qDebug() << "Клиент подключился:" << socketDescriptor;

    connect(clientSocket, &QTcpSocket::readyRead, this, [clientSocket]() {
        QByteArray data = clientSocket->readAll();
        qDebug() << "Получено:" << data;
        // Ответ клиенту
        auto str = data.toStdString();
        if (str == "OP1?\n" || str == "OP2?\n") {
            clientSocket->write("1\n");
        } else if (str == "I1?\n" || str == "I2?\n") {
            clientSocket->write("I1 10.0\n");
        } else if (str == "I1O?\n" || str == "I2O?\n") {
            clientSocket->write("A 5.0\n");
        } else if (str == "V1O?\n" || str == "V2O?\n") {
            clientSocket->write("V 24.0\n");
        } else {
            clientSocket->write("1\n");
        }
        clientSocket->flush();
    });

    connect(clientSocket, &QTcpSocket::disconnected, clientSocket,
            &QTcpSocket::deleteLater);
}
