#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

constexpr int NUMBER_OF_LAMPS = 6;
constexpr int NUMBER_OF_POWER_SUPPLIES = 3;
constexpr int MAX_CURRENT_LAMP_INDEX = 5;
constexpr int MIN_CURRENT_LAMP_INDEX = 0;

struct lamp{
    QString color;
    QString ip;
    double max_current;
    QString name;
    int out;
    double total_work_hours;
};

struct lamps_graphic_item_coords{
    int x;
    int y;
};

struct power_supply_properties{
    QString name;
    int x;
    int y;
};

struct lamps_powers_config{
    const int  max_current_lamp_index = MAX_CURRENT_LAMP_INDEX;
    const int  min_current_lamp_index = MIN_CURRENT_LAMP_INDEX;
    bool is_unclock = false;
    lamp lamps_array[NUMBER_OF_LAMPS];
    lamps_graphic_item_coords lamps_item_coords;
    power_supply_properties powers[NUMBER_OF_POWER_SUPPLIES];
};

namespace global{

// accuracy
extern const double kVoltageZeroAccuracy;
extern const double kCurrentTargetAccuracy;

// file names and dirs
extern const QString json_lamps_file_name;
extern const QString current_voltage_log_file_name;
extern const QString path_to_logs_dir;
extern const QString relative_path_to_logic_log_file;
extern const QString relative_path_to_cv_log_file;

// root objects
extern const char kJsonKeyLampsArray[];
extern const char kJsonKeyLampsCoordsObject[];
extern const char kJsonKeyPowerSupply1_Object[];
extern const char kJsonKeyPowerSupply2_Object[];
extern const char kJsonKeyPowerSupply3_Object[];

// common json keys
extern const char kJsonKeyIsUnlockKeyFlag[];
extern const char kJsonKeyName[];
extern const char kJsonKeyX[];
extern const char kJsonKeyY[];
extern const char kJsonKeyColor[];
extern const char kJsonKeyIp[];
extern const char kJsonKeyMaxCurrent[];
extern const char kJsonKeyMaxOut[];
extern const char kJsonKeyTotalWorkHours[];

}// end namespace global

#endif // CONFIG_H
