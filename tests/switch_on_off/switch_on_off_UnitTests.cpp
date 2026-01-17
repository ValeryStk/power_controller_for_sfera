#include "switch_on_off_UnitTests.h"

#include "power_supply_manager.h"
#include "config.h"
#include "text_log_constants.h"
#include <QDebug>


namespace{


} // end namespace


switch_on_off_UnitTests::switch_on_off_UnitTests()
{
}

void switch_on_off_UnitTests::initTestCase()
{
    // Инициализация перед запуском всех тестов
}

void switch_on_off_UnitTests::cleanupTestCase()
{
    // Очистка после выполнения всех тестов
}

void switch_on_off_UnitTests::init()
{
    // Инициализация перед каждым тестом

}

void switch_on_off_UnitTests::cleanup()
{
    // Очистка после каждого теста
}

void switch_on_off_UnitTests::switch_on_off_test()
{
    PowerSupplyManager ps;
    auto pwrs = ps.get_power_states();
    auto lamps = pwrs[global::kJsonKeyLampsArray].toArray();

    for(int i=0; i<lamps.size(); ++i){
        ps.decreaseVoltageStepByStepToZero(i);
    }

    for(int i=0; i<lamps.size(); ++i){
        auto voltage = ps.getVoltage(i);
        QString text_message = QString(tlc::kOperationSwitchOffResultFailed).arg(voltage);
        auto message = text_message.toStdString();
        QVERIFY2(voltage <= global::kVoltageZeroAccuracy, message.c_str());
    }

    for(int i=0; i<lamps.size(); ++i){
        ps.increaseVoltageStepByStepToCurrentLimit(i);
    }

    for(int i=0;i<lamps.size();++i){
        auto current = ps.getCurrentValue(i);
        auto current_limit = lamps[i].toObject()[global::kJsonKeyMaxCurrent].toDouble();
        auto diff = current_limit - current;
        QString text_message = QString(tlc::kOperationSwitchOnResultFailed).arg(current);
        auto message = text_message.toStdString();
        QVERIFY2(diff <= global::kCurrentTargetAccuracy, message.c_str());
    }
}

QTEST_MAIN(switch_on_off_UnitTests)
