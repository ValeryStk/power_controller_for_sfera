#include "config.h"

#include "QApplication"
#include "QDir"

namespace global {

// find power and out numbers for current_lamp_index
constexpr int lamp_pwr_out[NUMBER_OF_LAMPS][2] = {{1,1},
                                                  {1,2},
                                                  {2,1},
                                                  {2,2},
                                                  {3,1},
                                                  {3,2}};
constexpr int POWER_PARAM = 0;
constexpr int OUT_PARAM = 1;

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

int get_power_num_by_index(int index){
    if(index<0||index>=NUMBER_OF_LAMPS)return-1;
    return lamp_pwr_out[index][POWER_PARAM];
}

int get_power_out_by_index(int index){
    if(index<0||index>=NUMBER_OF_LAMPS)return-1;
    return lamp_pwr_out[index][OUT_PARAM];
}

void mayBe_create_log_dir()
{
    QString rootDir = QApplication::applicationDirPath() + global::path_to_logs_dir;
    QDir().mkpath(rootDir);
}

}
