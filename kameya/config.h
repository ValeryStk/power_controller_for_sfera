#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

namespace global{

// accuracy
extern const double kVoltageZeroAccuracy;
extern const double kCurrentTargetAccuracy;

// file names and dirs
extern const QString json_lamps_file_name;
extern const QString path_to_log_dir;
extern const QString relative_path_to_log_file;

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

}// end namespace global

#endif // CONFIG_H
