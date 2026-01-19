#include "config.h"

namespace global {

// accuracy
extern const double kVoltageZeroAccuracy    = 0.005;
extern const double kCurrentTargetAccuracy  = 0.03;

const QString json_lamps_file_name            = "ir_lamps.json";
const QString current_voltage_log_file_name   = "cv.log";
const QString path_to_logs_dir                = "/_logs";
const QString relative_path_to_logic_log_file = "/_logs/logic.log";
const QString relative_path_to_cv_log_file    = "/_logs/cv.log";

const char kJsonKeyLampsArray[]           = "lamps";
const char kJsonKeyLampsCoordsObject[]    = "lamps_item_coords";
const char kJsonKeyPowerSupply1_Object[]  = "ps1";
const char kJsonKeyPowerSupply2_Object[]  = "ps2";
const char kJsonKeyPowerSupply3_Object[]  = "ps3";

const char kJsonKeyIsUnlockKeyFlag[]      = "is_unlock";
const char kJsonKeyName[]                 = "name";
const char kJsonKeyX[]                    = "x";
const char kJsonKeyY[]                    = "y";
const char kJsonKeyColor[]                = "color";
const char kJsonKeyIp[]                   = "ip";
const char kJsonKeyMaxCurrent[]           = "max_current";
const char kJsonKeyMaxOut[]               = "out";
const char kJsonKeyTotalWorkHours[]       = "total_work_hours";
}
