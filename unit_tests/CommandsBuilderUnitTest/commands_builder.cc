#include "commands_builder.h"
#include <sstream>
#include <iostream>
#include <typeinfo>

namespace {

const char deviceID[] =    "*IDN?";
const char switchOnAll[] =  "OPALL 1";
const char switchOffAll[] = "OPALL 0";
const char switchUnit[] =   "OP";
const char voltage[] =      "V";
const char current[] =      "I";

template<typename T>
string to_string(int prec,T value){
std::stringstream stream;
stream.precision(prec);
//stream << std::fixed;
stream << value;
return stream.str();
}

template<typename T>
string makeCommand(const string &command, const uint16_t &unit,
                   const T &value) {
    const string unitStr =  std::to_string(unit);
    string valueStr;
    valueStr = to_string(3,value);
    string commandStr;
    commandStr.append(command);
    commandStr.append(unitStr);
    commandStr.append(" ");
    commandStr.append(valueStr);
    commandStr.append("\n");
    return commandStr;
}

string makeCommand(const string &command, const uint16_t &unit) {

    const string unitStr = std::to_string(unit);
    string commandStr;
    commandStr.append(command);
    commandStr.append(unitStr);
    commandStr.append("?\n");
    return commandStr;
}

string makeReadBackCommand(const string &command,
                           const uint16_t &unit) {

    const string unitStr = std::to_string(unit);
    string commandStr;
    commandStr.append(command);
    commandStr.append(unitStr);
    commandStr.append("O?\n");
    return commandStr;
};

} // end of namespace

namespace pwr {

string switch_on_all()
{    
    return string(switchOnAll);
}

string switch_off_all()
{
    return string(switchOffAll);
}

string switch_on(pwr::unit unit)
{
    return makeCommand(switchUnit, unit, 1);
}

string switch_off(pwr::unit unit)
{
    return makeCommand(switchUnit, unit, 0);
}

string set_I_limit(pwr::unit unit, const float &value)
{
    return makeCommand(current, unit, value);
}

string get_I_limit(pwr::unit unit)
{
    return makeCommand(current, unit);
}

string get_I_value(pwr::unit unit)
{
    return makeReadBackCommand(current, unit);
}

string get_V_value(pwr::unit unit)
{
    return makeReadBackCommand(voltage, unit);
}

string set_V_value(pwr::unit unit, const float &value)
{
    return makeCommand(voltage, unit, value);
}

string get_V_trip(pwr::unit unit)
{
    return makeCommand(voltage, unit);
}

string get_switch(pwr::unit unit)
{
    return makeCommand(switchUnit, unit);
}

string get_device_id()
{
    return deviceID;
}

} // end namespace pwr
