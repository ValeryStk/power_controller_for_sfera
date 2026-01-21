#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

constexpr int NUMBER_OF_LAMPS = 6;
constexpr int NUMBER_OF_POWER_SUPPLIES = 3;
constexpr int MAX_CURRENT_LAMP_INDEX = 5;
constexpr int MIN_CURRENT_LAMP_INDEX = 0;
constexpr int MAX_VOLTAGE = 25;
constexpr int MAX_CURRENT_LIMIT = 11;
constexpr double VOLTAGE_INCREASE_STEP = 0.25;
constexpr double VOLTAGE_DECREASE_STEP = 0.25;

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
    const int  max_voltage = MAX_VOLTAGE;
    const int  max_current_limit = MAX_CURRENT_LIMIT;
    const double  voltage_increasing_step = VOLTAGE_INCREASE_STEP;
    const double  voltage_decreasing_step = VOLTAGE_DECREASE_STEP;
    bool is_json_config_valid = false;
    bool is_sound = false;
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
extern const QString json_lamps_qrc_file_name;
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
extern const char kJsonIsSoundFlag[];
extern const char kJsonKeyIsUnlockKeyFlag[];
extern const char kJsonKeyName[];
extern const char kJsonKeyX[];
extern const char kJsonKeyY[];
extern const char kJsonKeyColor[];
extern const char kJsonKeyIp[];
extern const char kJsonKeyMaxCurrent[];
extern const char kJsonKeyOut[];
extern const char kJsonKeyTotalWorkHours[];

void mayBe_create_log_dir();

int get_power_num_by_index(int index);

int get_power_out_by_index(int index);

void get_config_struct(const QString path_to_json_config,
                       lamps_powers_config &cfg);

}// end namespace global

#endif // CONFIG_H
