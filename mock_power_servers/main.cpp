#include <MainWindow.h>

#include <QApplication>
#include <QDebug>
#include <QPushButton>
#include <QThread>

#include "mock_power_server.h"

// Обработчик сообщений
void myMessageHandler(QtMsgType type, const QMessageLogContext &context,
                      const QString &msg) {
    QString txt;
    switch (type) {
        case QtDebugMsg:
            txt = QString("Debug: %1").arg(msg);
            break;
        case QtInfoMsg:
            txt = QString("Info: %1").arg(msg);
            break;
        case QtWarningMsg:
            txt = QString("Warning: %1").arg(msg);
            break;
        case QtCriticalMsg:
            txt = QString("Critical: %1").arg(msg);
            break;
        case QtFatalMsg:
            txt = QString("Fatal: %1").arg(msg);
            abort();
    }

    if (MainWindow::instance && MainWindow::instance->logWindow) {
        MainWindow::instance->logWindow->appendPlainText(txt);
    }
}

int main(int argc, char *argv[]) {
    QThread serverThread_1;
    MockPowerServer *server_1 = new MockPowerServer();
    server_1->moveToThread(&serverThread_1);

    QThread serverThread_2;
    MockPowerServer *server_2 = new MockPowerServer();
    server_2->moveToThread(&serverThread_2);

    QThread serverThread_3;
    MockPowerServer *server_3 = new MockPowerServer();
    server_3->moveToThread(&serverThread_3);

    QObject::connect(&serverThread_1, &QThread::started, [server_1]() {
        QHostAddress bindAddress("127.0.0.1");
        if (!server_1->listen(bindAddress, 9221)) {
            qDebug() << "Не удалось запустить сервер!";
        } else {
            qDebug() << "MockPowerServer слушает на IP:"
                     << bindAddress.toString()
                     << "порт:" << server_1->serverPort();
        }
    });

    QObject::connect(&serverThread_1, &QThread::finished, server_1,
                     &QObject::deleteLater);
    serverThread_1.start();

    QObject::connect(&serverThread_2, &QThread::started, [server_2]() {
        QHostAddress bindAddress("127.0.0.2");
        if (!server_2->listen(bindAddress, 9221)) {
            qDebug() << "Не удалось запустить сервер!";
        } else {
            qDebug() << "MockPowerServer слушает на IP:"
                     << bindAddress.toString()
                     << "порт:" << server_2->serverPort();
        }
    });

    QObject::connect(&serverThread_2, &QThread::finished, server_2,
                     &QObject::deleteLater);
    serverThread_2.start();

    QObject::connect(&serverThread_3, &QThread::started, [server_3]() {
        QHostAddress bindAddress("127.0.0.3");
        if (!server_3->listen(bindAddress, 9221)) {
            qDebug() << "Не удалось запустить сервер!";
        } else {
            qDebug() << "MockPowerServer слушает на IP:"
                     << bindAddress.toString()
                     << "порт:" << server_3->serverPort();
        }
    });

    QObject::connect(&serverThread_3, &QThread::finished, server_3,
                     &QObject::deleteLater);
    serverThread_3.start();

    QApplication app(argc, argv);

    MainWindow window;
    window.show();
    qInstallMessageHandler(myMessageHandler);
    return app.exec();
}
