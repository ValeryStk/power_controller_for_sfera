#include "switch_on_off_UnitTests.h"

#include <QDebug>

#include "config.h"
#include "power_supply_manager.h"
#include "qrc_files_restorer.h"
#include "text_log_constants.h"

namespace {}  // end namespace

switch_on_off_UnitTests::switch_on_off_UnitTests() {}

void switch_on_off_UnitTests::initTestCase() {
    // Инициализация перед запуском всех тестов
    utils::restoreFilesFromDefaultQrc();
}

void switch_on_off_UnitTests::cleanupTestCase() {
    // Очистка после выполнения всех тестов
}

void switch_on_off_UnitTests::init() {
    // Инициализация перед каждым тестом
}

void switch_on_off_UnitTests::cleanup() {
    // Очистка после каждого теста
}

void switch_on_off_UnitTests::switch_on_off_test() {
    qDebug() << QString(tlc::kPowerManagerConstructor);
    PowerSupplyManager ps;
    ps.initSocket();
    lamps_powers_config cfg;
    global::get_config_struct(cfg);
}

QTEST_MAIN(switch_on_off_UnitTests)
