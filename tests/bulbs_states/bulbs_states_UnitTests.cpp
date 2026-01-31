#include "bulbs_states_UnitTests.h"

#include <QDebug>

#include "MainWindow.h"
#include "QCoreApplication"
#include "QFile"
#include "config.h"
#include "logger.h"
#include "qrc_files_restorer.h"
#include "ui_MainWindow.h"  // IWYU pragma: keep

namespace {}  // end namespace

bulbs_states_UnitTests::bulbs_states_UnitTests() {}

void bulbs_states_UnitTests::initTestCase() {
    // Инициализация перед запуском всех тестов
    utils::restoreFilesFromQrc(":/mock");
    QString app_path = QCoreApplication::applicationDirPath() + "/";
    QFile::rename(app_path + "mock_config.json",
                  app_path + global::config_json_file_name);
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
