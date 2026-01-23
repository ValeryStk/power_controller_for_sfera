#include "json_config_to_struct_UnitTests.h"

#include <QDebug>

#include "config.h"

namespace {}  // end namespace

json_config_to_struct_UnitTests::json_config_to_struct_UnitTests() {}

void json_config_to_struct_UnitTests::initTestCase() {
    // Инициализация перед запуском всех тестов
}

void json_config_to_struct_UnitTests::cleanupTestCase() {
    // Очистка после выполнения всех тестов
}

void json_config_to_struct_UnitTests::init() {
    // Инициализация перед каждым тестом
}

void json_config_to_struct_UnitTests::cleanup() {
    // Очистка после каждого теста
}

void json_config_to_struct_UnitTests::json_to_struct_test() {
    lamps_powers_config cfg;
    global::get_config_struct(cfg);
    for (int i = 0; i < NUMBER_OF_LAMPS; ++i) {
        qDebug() << cfg.lamps_array[i].name;
    }
    for (int i = 0; i < NUMBER_OF_LAMPS; ++i) {
        qDebug() << cfg.lamps_array[i].ip;
    }
}

QTEST_MAIN(json_config_to_struct_UnitTests)
