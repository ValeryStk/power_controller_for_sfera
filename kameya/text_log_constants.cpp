#include "text_log_constants.h"  // IWYU pragma: keep

namespace tlc {

const char kPowerManagerConstructor[] = "power manager constructor";
const char kPowerManagerDestructor[] = "power manager destructor";

const char kStartTheLog[] =
    "************************************************************\n"
    "******************** POWER CONTROLLER **********************\n"
    "************************************************************";
const char kEndOfTheLog[] =
    "############################################################\n"
    "#################### END OF THE LOG ########################\n"
    "############################################################\n";

const char kOperationUpdateAllPowersStates[] =
    "RETEST ALL POWERS STATES OPERATION";
const char kOperatinAllLampsSwitchOffName[] = "SWITCH OFF ALL LAMPS OPERATION";
const char kOperationAllLampsSwitchOffFailed[] =
    "SWITCH OFF ALL LAMPS OPERATION FAILED ---> POWER %1 is DISCONNECTED";
const char kOperatinAllLampsSwitchOnName[] = "SWITCH ON  ALL LAMPS OPERATION";
const char kOperationAllLampsSwitchOnFailed[] =
    "SWITCH ON  ALL LAMPS OPERATION FAILED ---> POWER %1 is DISCONNECTED";
const char kOperationSwitchOffOneLampName[] = "SWITCH OFF ONE LAMP OPERATION";
const char kOperationSwitchOffOneLampFailed[] =
    "SWITCH OFF ONE LAMP FAILED ---> POWER %1 is DISCONNECTED";
const char kOperationSwitchOnOneLampName[] = "SWITCH ON  ONE LAMP OPERATION";
const char kOperationSwitchOnOneLampFailed[] =
    "SWITCH ON  ONE LAMP FAILED ---> POWER %1 is DISCONNECTED";
const char kOperationSwitchOffResultFailed[] =
    "SWITCH OFF OPERATION FAILED (VOLTAGE IS NOT ZERO (%1))";
const char kOperationSwitchOnResultFailed[] =
    "SWITCH ON  OPERATION FAILED (TARGET CURRENT IS NOT REACHED (%1))";
const char kFailedLampIndex[] = "FAILED LAMP: %1";
const char kFailIncreasingProcessSocketUnconnected[] =
    "INCREASING LAMP %1 FAILED BECAUSE QTCPSOCKET::UNCONNECTED";
const char kFailDecreasingProcessSocketUnconnected[] =
    "DECREASING LAMP %1 FAILED BECAUSE QTCPSOCKET::UNCONNECTED";
const char kFailDecreasingProcessVoltageUnchaged[] =
    "POWER %1 OUT %2 IS ON BUT VOLTAGE IS NOT POSSIBLE TO DECREASE";

const char kStateMachineWaitCommandState[] = "ОЖИДАНИЕ КОМАНДЫ";
const char kStateMachineOneLampOnCommandState[] = "ВКЛЮЧЕНИЕ ЛАМПЫ %1";
const char kStateMachineOneLampOffCommandState[] = "ВЫКЛЮЧЕНИЕ ЛАМПЫ %1";
const char kStateMachineAllLampsOnCommandState[] = "ВКЛЮЧЕНИЕ ВСЕХ ЛАМП";
const char kStateMachineAllLampsOffCommandState[] = "ВЫКЛЮЧЕНИЕ ВСЕХ ЛАМП";
const char kStateMachineUpdateAllLampsCommandState[] =
    "ОБНОВЛЕНИЕ СОСТОЯНИЯ ВСЕХ БЛОКОВ";

}  // end namespace tlc
