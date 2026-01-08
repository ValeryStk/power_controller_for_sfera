#include "switch_on_off_UnitTests.h"

#include "power_supply_manager.h"

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
    auto lamps = pwrs["lamps"].toArray();
    for(int j=0;j<2;++j){

        for(int i=0;i<lamps.size();++i){
            ps.decreaseVoltageStepByStepToZero(i);
        }
        for(int i=0;i<lamps.size();++i){
            auto voltage = ps.getVoltage(i);
            QVERIFY2(voltage<=0.005,"voltage zero check");
        }
        for(int i=0;i<lamps.size();++i){
            ps.increaseVoltageStepByStepToCurrentLimit(i);
        }

        for(int i=0;i<lamps.size();++i){
            auto current = ps.getCurrentValue(i);
            auto current_limit = ps.getCurrentLimit(i);
            auto diff = current_limit - current;
            QVERIFY2(diff<=0.03,"voltage zero check");
        }

    }

}



QTEST_MAIN(switch_on_off_UnitTests)
