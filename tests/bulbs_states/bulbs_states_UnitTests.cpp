#include "bulbs_states_UnitTests.h"

#include <QDebug>

#include "MainWindow.h"
#include "logger.h"
#include "mock_power_server.h"
#include "qrc_files_restorer.h"
#include "ui_MainWindow.h"

namespace {}  // end namespace

bulbs_states_UnitTests::bulbs_states_UnitTests() {}

void bulbs_states_UnitTests::initTestCase() {
    // Инициализация перед запуском всех тестов
    QrcFilesRestorer::restoreFilesFromQrc(":/4restoring");
}

void bulbs_states_UnitTests::cleanupTestCase() {
    // Очистка после выполнения всех тестов
}

void bulbs_states_UnitTests::init() {
    // Инициализация перед каждым тестом
}

void bulbs_states_UnitTests::cleanup() {
    // Очистка после каждого теста
}

void bulbs_states_UnitTests::bulbs_items_test() {
    int argc = 0;
    QApplication app(argc, nullptr);
    QThread serverThread;
    MockPowerServer *server = new MockPowerServer();

    // Переносим сервер в отдельный поток
    server->moveToThread(&serverThread);

    QObject::connect(&serverThread, &QThread::started, [server]() {
        // Сервер слушает только на IP 192.168.0.100
        QHostAddress bindAddress("127.0.0.1");
        if (!server->listen(bindAddress, 9221)) {
            qDebug() << "Не удалось запустить сервер!";
        } else {
            qDebug() << "MockPowerServer слушает на IP:"
                     << bindAddress.toString()
                     << "порт:" << server->serverPort();
        }
    });

    QObject::connect(&serverThread, &QThread::finished, server,
                     &QObject::deleteLater);
    serverThread.start();
    QPixmap pixmap = QPixmap::fromImage(QImage(":/guiPictures/bug.svg"));
    QCursor customCursor(pixmap);
    QApplication::setOverrideCursor(customCursor);
    qInstallMessageHandler(myMessageOutput);
    MainWindow *main_window = new MainWindow;
    main_window->show();
    app.exec();
}

QTEST_MAIN(bulbs_states_UnitTests)
