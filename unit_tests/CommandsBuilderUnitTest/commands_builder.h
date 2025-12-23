#ifndef COMMANDS_BUILDER_H
#define COMMANDS_BUILDER_H

#include <string>

using std::string;

namespace pwr {
enum unit {
    OUT_1 = 1,
    OUT_2
};

string switch_on_all();
string switch_off_all();
string switch_on  (pwr::unit unit);
string switch_off (pwr::unit unit);
string set_I_limit(pwr::unit unit,const float &value);
string get_I_limit(pwr::unit unit);
string get_I_value(pwr::unit unit);
string get_V_value(pwr::unit unit);
string set_V_value(pwr::unit unit, const float &value);
string get_V_trip (pwr::unit unit);
string get_switch (pwr::unit unit);
string get_device_id();

}

#endif // COMMANDS_BUILDER_H
