#include "config.h"

#include "QApplication"
#include "QDir"
#include "json_utils.h"
#include "qjsonarray.h"
#include "qjsonobject.h"

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
const double kVoltageZeroAccuracy    = 0.005;
const double kCurrentTargetAccuracy  = 0.03;

const QString json_lamps_file_name            = "ir_lamps.json";
const QString json_lamps_qrc_file_name        = ":/4restoring/ir_lamps.json";
const QString current_voltage_log_file_name   = "cv.log";
const QString path_to_logs_dir                = "/_logs";
const QString relative_path_to_logic_log_file = "/_logs/logic.log";
const QString relative_path_to_cv_log_file    = "/_logs/cv.log";

const char kJsonKeyLampsArray[]           = "lamps";
const char kJsonKeyLampsCoordsObject[]    = "lamps_item_coords";
const char kJsonKeyPowerSupply1_Object[]  = "ps1";
const char kJsonKeyPowerSupply2_Object[]  = "ps2";
const char kJsonKeyPowerSupply3_Object[]  = "ps3";

const char kJsonIsSoundFlag[]             = "is_sound";
const char kJsonKeyIsUnlockKeyFlag[]      = "is_unlock";
const char kJsonKeyName[]                 = "name";
const char kJsonKeyX[]                    = "x";
const char kJsonKeyY[]                    = "y";
const char kJsonKeyColor[]                = "color";
const char kJsonKeyIp[]                   = "ip";
const char kJsonKeyMaxCurrent[]           = "max_current";
const char kJsonKeyOut[]                  = "out";
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

void get_config_struct(const QString path_to_json_config,
                       lamps_powers_config& cfg)
{
    QJsonObject jo;
    jsn::getJsonObjectFromFile(json_lamps_file_name, jo);
    cfg.is_sound     = jo[kJsonIsSoundFlag].toBool();
    cfg.is_unclock   = jo[kJsonKeyIsUnlockKeyFlag].toBool();
    QJsonArray lamps    = jo[kJsonKeyLampsArray].toArray();
    bool is_valid = true;
    if(lamps.size()!=NUMBER_OF_LAMPS){
    cfg.is_json_config_valid = false;
    return;
    };
    for(int i=0;i<NUMBER_OF_LAMPS;++i){
        QJsonObject jlamp = lamps[i].toObject();
        lamp lmp;
        lmp.color             = jlamp[kJsonKeyColor].toString();
        lmp.ip                = jlamp[kJsonKeyIp].toString();
        lmp.max_current       = jlamp[kJsonKeyMaxCurrent].toDouble();
        lmp.name              = jlamp[kJsonKeyName].toString();
        lmp.out               = jlamp[kJsonKeyOut].toInt();
        lmp.total_work_hours  = jlamp[kJsonKeyTotalWorkHours].toDouble();
        cfg.lamps_array[i] = lmp;
    }
    auto lamp_iten_coord = jo[kJsonKeyLampsCoordsObject].toObject();
    cfg.lamps_item_coords.x = lamp_iten_coord[kJsonKeyX].toInt();
    cfg.lamps_item_coords.y = lamp_iten_coord[kJsonKeyY].toInt();

    auto power_supply_1 = jo[kJsonKeyPowerSupply1_Object].toObject();
    auto power_supply_2 = jo[kJsonKeyPowerSupply2_Object].toObject();
    auto power_supply_3 = jo[kJsonKeyPowerSupply3_Object].toObject();

    QJsonArray jpowers = {power_supply_1,power_supply_2,power_supply_3};

    for(int i=0;i<NUMBER_OF_POWER_SUPPLIES;++i){
      cfg.powers[i].name =  jpowers[i].toObject()[kJsonKeyName].toString();
      cfg.powers[i].x    =  jpowers[i].toObject()[kJsonKeyX].toInt();
      cfg.powers[i].y    =  jpowers[i].toObject()[kJsonKeyY].toInt();
    }
    cfg.is_json_config_valid = is_valid;
}

} // end namespace global
