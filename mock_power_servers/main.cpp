#include <QCoreApplication>
#include <QDebug>
#include <QHostAddress>
#include <QThread>

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

    // Список IP-адресов для серверов
    QStringList addresses = {"127.0.0.1", "127.0.0.2", "127.0.0.3"};
    QList<QThread *> threads;

    for (const QString &addr : addresses) {
        QThread *thread = new QThread;
        MockPowerServer *server = new MockPowerServer();
        server->moveToThread(thread);

        QObject::connect(thread, &QThread::started, [server, addr]() {
            QHostAddress bindAddress(addr);
            if (!server->listen(bindAddress, 9221)) {
                qDebug() << "Не удалось запустить сервер на" << addr;
            } else {
                qDebug() << "MockPowerServer listen IP:"
                         << bindAddress.toString()
                         << "port:" << server->serverPort();
            }
        });
        QObject::connect(thread, &QThread::finished, server,
                         &QObject::deleteLater);

        thread->start();
        threads << thread;
    }

    // Корректное завершение потоков при выходе
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&]() {
        int i = 0;
        for (QThread *thread : qAsConst(threads)) {
            thread->quit();
            thread->wait();
            delete thread;
            qInfo() << "stop server: " << ++i;
        }
    });

    return app.exec();
}
