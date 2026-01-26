#ifndef TEXT_LOG_CONSTANTS_H
#define TEXT_LOG_CONSTANTS_H

namespace tlc {

extern const char kPowerManagerConstructor[];
extern const char kPowerManagerDestructor[];
extern const char kStartTheLog[];
extern const char kEndOfTheLog[];

extern const char kOperationUpdateAllPowersStates[];

extern const char kOperatinAllLampsSwitchOffName[];
extern const char kOperationAllLampsSwitchOffFailed[];

extern const char kOperatinAllLampsSwitchOnName[];
extern const char kOperationAllLampsSwitchOnFailed[];

extern const char kOperationSwitchOffOneLampName[];
extern const char kOperationSwitchOffOneLampFailed[];

extern const char kOperationSwitchOnOneLampName[];
extern const char kOperationSwitchOnOneLampFailed[];

extern const char kOperationSwitchOffResultFailed[];
extern const char kOperationSwitchOnResultFailed[];

extern const char kFailedLampIndex[];

extern const char kFailIncreasingProcessSocketUnconnected[];
extern const char kFailDecreasingProcessSocketUnconnected[];
extern const char kFailDecreasingProcessVoltageUnchaged[];

/*enum class CONTROLLER_STATES {
    WAIT_COMMAND,
    ONE_LAMP_SWITCH_OFF_PROCESS,
    ONE_LAMP_SWITCH_ON_PROCESS,
    ALL_LAMPS_SWITCH_OFF_PROCESS,
    ALL_LAMPS_SWITCH_ON_PROCESS,
    UPDATE_ALL_STATES_PROCESS
};*/

extern const char kStateMachineWaitCommandState[];
extern const char kStateMachineOneLampOnCommandState[];
extern const char kStateMachineOneLampOffCommandState[];
extern const char kStateMachineAllLampsOnCommandState[];
extern const char kStateMachineAllLampsOffCommandState[];
extern const char kStateMachineUpdateAllLampsCommandState[];

}  // namespace tlc

#endif  // TEXT_LOG_CONSTANTS_H
