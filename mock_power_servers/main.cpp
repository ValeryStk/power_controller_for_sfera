#include <QCoreApplication>
#include <QDebug>
#include <QHostAddress>

#include "mock_power_server.h"

// Обработчик сообщений — выводим только в консоль
void myMessageHandler(QtMsgType type, const QMessageLogContext &,
                      const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    const char *prefix = "";
    switch (type) {
        case QtDebugMsg:
            prefix = "Debug";
            break;
        case QtInfoMsg:
            prefix = "Info";
            break;
        case QtWarningMsg:
            prefix = "Warning";
            break;
        case QtCriticalMsg:
            prefix = "Critical";
            break;
        case QtFatalMsg:
            prefix = "Fatal";
            break;
    }
    fprintf(stderr, "%s: %s\n", prefix, localMsg.constData());
    if (type == QtFatalMsg) abort();
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    qInstallMessageHandler(myMessageHandler);

    if (argc < 2) {
        qCritical() << "Usage: mock_server <IP>";
        return 1;
    }

    QString addr = argv[1];
    QHostAddress bindAddress(addr);

    MockPowerServer server;
    if (!server.listen(bindAddress, 9221)) {
        qCritical() << "Не удалось запустить сервер на" << addr;
        return 1;
    } else {
        qInfo() << "MockPowerServer listen IP:" << bindAddress.toString()
                << "port:" << server.serverPort();
    }

    return app.exec();
}
