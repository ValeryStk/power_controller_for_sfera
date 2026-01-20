#include "mock_power_server.h"
#include <QDebug>

MockPowerServer::MockPowerServer(QObject *parent)
    : QTcpServer(parent) {}

void MockPowerServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    qDebug() << "Клиент подключился:" << socketDescriptor;

    connect(clientSocket, &QTcpSocket::readyRead, this, [clientSocket]() {
        QByteArray data = clientSocket->readAll();
        qDebug() << "Получено:" << data;
        // Ответ клиенту
        const char* res = "1\n";
        clientSocket->write(res);
        clientSocket->flush();
    });

    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
}

