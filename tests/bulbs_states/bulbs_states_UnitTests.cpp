#include "bulbs_states_UnitTests.h"

#include <QDebug>

#include "MainWindow.h"
#include "config.h"
#include "logger.h"
#include "mock_power_server.h"
#include "ui_MainWindow.h"  // IWYU pragma: keep

namespace {}  // end namespace

bulbs_states_UnitTests::bulbs_states_UnitTests() {}

void bulbs_states_UnitTests::initTestCase() {
    // Инициализация перед запуском всех тестов
    QFileInfo fi(global::config_json_file_name);
    QFile::copy(":/mock/mock_config.json", global::config_json_file_name);
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
    lamps_powers_config cfg;
    global::get_config_struct(":/mock/mock_config.json", cfg);
    QApplication app(argc, nullptr);

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

    QPixmap pixmap = QPixmap::fromImage(QImage(":/svg/red_bug.svg"));
    QCursor customCursor(pixmap);
    QApplication::setOverrideCursor(customCursor);
    QApplication::setWindowIcon((QIcon(":/bulbs_test.ico")));
    qInstallMessageHandler(myMessageOutput);
    MainWindow *main_window = new MainWindow;
    main_window->setWindowTitle("BULB STATES TEST");
    main_window->show();
    app.exec();
}

QTEST_MAIN(bulbs_states_UnitTests)
