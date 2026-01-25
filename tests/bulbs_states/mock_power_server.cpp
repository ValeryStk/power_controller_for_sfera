#include "mock_power_server.h"

#include <QDebug>

MockPowerServer::MockPowerServer(QObject *parent) : QTcpServer(parent) {}

void MockPowerServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    qDebug() << "Клиент подключился:" << socketDescriptor;

    connect(clientSocket, &QTcpSocket::readyRead, this, [clientSocket]() {
        QByteArray data = clientSocket->readAll();
        static double V = 24.0;
        static double I = 10.0;
        static double Ilim = 10.01;
        int state = 1;

        qDebug() << "request: " << data;
        QString response;
        auto str = QString::fromStdString(data.toStdString());
        if (str == "OP1?\n" || str == "OP2?\n") {
            response = QString("%1\n").arg(state);
            clientSocket->write(response.toUtf8());
            clientSocket->flush();
            qDebug() << "response: " << response;
            return;
        } else if (str == "I1?\n" || str == "I2?\n") {
            QString tmp = str.remove("?\n");
            clientSocket->write(QString("%1 %2\n").arg(tmp).arg(Ilim).toUtf8());
            clientSocket->flush();
            return;
        } else if (str == "I1O?\n" || str == "I2O?\n") {
            clientSocket->write(QString("I1 %1\n").arg(10).toUtf8());
            clientSocket->flush();
            return;
        } else if (str == "V1O?\n" || str == "V2O?\n") {
            QString tmp = str.remove("O?\n");
            clientSocket->write(QString("%1 %2\n").arg(tmp).arg(V).toUtf8());
            clientSocket->flush();
            return;
        } else if (str.contains("V1") || str.contains("V2")) {
            I = I + 1;
            QString tmp = str;
            bool ok;
            double value =
                tmp.remove("V1").remove("V2").remove(" ").remove("\n").toDouble(
                    &ok);
            qDebug() << "V value: " << value;
            clientSocket->write("1\n");
            if (ok) {
            }
        } else {
            qDebug() << "Uknown command:" << str;
            clientSocket->write("1\n");
        }
        clientSocket->flush();
    });

    connect(clientSocket, &QTcpSocket::disconnected, clientSocket,
            &QTcpSocket::deleteLater);
}
