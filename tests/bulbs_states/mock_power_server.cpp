#include "mock_power_server.h"

#include <QDebug>

MockPowerServer::MockPowerServer(QObject *parent) : QTcpServer(parent) {}

void MockPowerServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    qDebug() << "Клиент подключился:" << socketDescriptor;

    connect(clientSocket, &QTcpSocket::readyRead, this, [clientSocket, this]() {
        QByteArray data = clientSocket->readAll();
        int state = 1;

        qDebug() << "request: " << data;
        QString response;
        auto str = QString::fromStdString(data.toStdString());
        int out;
        QString tmp = str;
        if (tmp.contains("?")) {
            qDebug() << "GET MESSAGES: " << tmp;
            tmp.remove("?").remove("\n");
            if (tmp.contains("V")) {
                tmp.remove("V");
                tmp.remove("O");
                bool ok;
                out = tmp.toInt(&ok);
                qDebug() << "GET V for out " << out << tmp << ok;
                double value;
                if (out == 1) value = Voltage_out_1;
                if (out == 2) value = Voltage_out_2;
                clientSocket->write(
                    QString("V%1 %2\n").arg(out).arg(value).toUtf8());
                clientSocket->flush();
                return;
            } else if (tmp.contains("I")) {
                tmp.remove("I");
                double value;
                bool ok = tmp.contains("O");
                if (ok) {
                    tmp.remove("O");
                    out = tmp.toInt(&ok);
                    if (out == 1) value = I_current_out_1;
                    if (out == 2) value = I_current_out_2;

                } else {
                    out = tmp.toInt(&ok);
                    qDebug() << "Ok I lim parser" << ok;
                    if (out == 1) value = I_limit_out_1;
                    if (out == 2) value = I_limit_out_2;
                }
                qDebug() << "GET I for out " << out << tmp << ok;
                clientSocket->write(QString("%1\n").arg(value).toUtf8());
                clientSocket->flush();
                return;
            }

        } else {
            qDebug() << "SET MESSAGES: " << tmp;
            if (tmp.contains("V")) {
                tmp.remove("V").remove("\n");
                qDebug() << "SET V message: " << tmp;
                QStringList values = tmp.split(" ");
                out = values[0].toInt();
                double value = values[1].toDouble();
                if (out == 1) {
                    Voltage_out_1 = value;
                    I_current_out_1 = Voltage_out_1 / 2.4;
                } else if (out == 2) {
                    Voltage_out_2 = value;
                    I_current_out_2 = Voltage_out_2 / 2.4;
                }
                return;
            }
        }

        if (str == "OP1 1\n" || str == "OP2 1\n") {
            response = QString("%1\n").arg(state);
            clientSocket->flush();
            qDebug() << "response: "
                     << "ignore";
            return;

        } else if (str == "OP1?\n" || str == "OP2?\n") {
            response = QString("%1\n").arg(state);
            clientSocket->write(response.toUtf8());
            clientSocket->flush();
            qDebug() << "response: " << response;
            return;
        }
    });
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket,
            &QTcpSocket::deleteLater);
}
