#include <QtTest>
#include "../../kameya/CommandsBuilder.h"
#include "commands_builder.h"

class CommandsBuilderTest : public QObject
{
    Q_OBJECT

public:
    CommandsBuilderTest();
    ~CommandsBuilderTest();

private slots:
    void switchOnAll_case();
    void switchOffAll_case();
    void switchOn_case  ();
    void switchOff_case ();
    void set_I_limit_case();
    void get_I_limit_case();
    void get_I_value_case();
    void get_V_value_case();
    void set_V_value_case();
    void get_V_trip_case();
    void get_switch_case ();
    void get_device_id_case();

};

CommandsBuilderTest::CommandsBuilderTest()
{

}

CommandsBuilderTest::~CommandsBuilderTest()
{

}

void CommandsBuilderTest::switchOnAll_case()
{
    CommandsBuilder cb;
    QVERIFY(cb.makeSwitchOnAllunitsCommand().constData() == pwr::switch_on_all());
}

void CommandsBuilderTest::switchOffAll_case()
{
    CommandsBuilder cb;
    QVERIFY(cb.makeSwitchOffAllunitsCommand().constData() == pwr::switch_off_all());
}

void CommandsBuilderTest::switchOn_case()
{
    CommandsBuilder cb;
    QVERIFY(cb.makeSwitchOnUnitCommand(1).constData() == pwr::switch_on(pwr::OUT_1));
}

void CommandsBuilderTest::switchOff_case()
{
    CommandsBuilder cb;
    QVERIFY(cb.makeSwitchOffUnitCommand(1).constData() == pwr::switch_off(pwr::OUT_1));
    QVERIFY(cb.makeSwitchOffUnitCommand(2).constData() == pwr::switch_off(pwr::OUT_2));
    QVERIFY(cb.makeSwitchOffUnitCommand(3).constData() != pwr::switch_off(pwr::OUT_2));
}

void CommandsBuilderTest::set_I_limit_case()
{
    CommandsBuilder cb;
    QVERIFY(cb.makeSetCurrentLimitCommand(1,0.800f).constData() == pwr::set_I_limit(pwr::OUT_1,0.80f));
    QVERIFY(cb.makeSetCurrentLimitCommand(2,0.847f).constData() == pwr::set_I_limit(pwr::OUT_2,0.847f));
    QVERIFY(cb.makeSetCurrentLimitCommand(2,6).constData() == pwr::set_I_limit(pwr::OUT_2,6));
}

void CommandsBuilderTest::get_I_limit_case()
{

}

void CommandsBuilderTest::get_I_value_case()
{

}

void CommandsBuilderTest::get_V_value_case()
{

}

void CommandsBuilderTest::set_V_value_case()
{

}

void CommandsBuilderTest::get_V_trip_case()
{

}

void CommandsBuilderTest::get_switch_case()
{

}

void CommandsBuilderTest::get_device_id_case()
{

}

QTEST_APPLESS_MAIN(CommandsBuilderTest)

#include "tst_commandsbuildertest.moc"
