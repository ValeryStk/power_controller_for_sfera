#ifndef COMMANDS_BUILDER_H
#define COMMANDS_BUILDER_H

#include <QByteArray>
#include <QString>
#include <cstdint>

namespace pwr {

extern const QString deviceID = "*IDN?";
extern const QString switchOnAll = "OPALL 1";
extern const QString switchOffAll = "OPALL 0";
extern const QString switchUnit = "OP";
extern const QString voltage = "V";
extern const QString current = "I";
extern const QString deltaI = "DELTAI";
extern const QString deltaV = "DELTAV";
extern const QString incrementI = "INCI";
extern const QString incrementV = "INCV";
extern const QString decrementI = "DECI";
extern const QString decrementV = "DECV";

inline QByteArray makeGetDeviceID_Command() {
  return deviceID.toUtf8();
}

inline QByteArray makeCommand(const QString& command, const quint16& unit,
                              const float& value) {
  QString unitStr = QString::number(unit);
  QString valueStr = QString::number(value);
  QString commandStr = QString("%1%2 %3\n").arg(command, unitStr, valueStr);
  return commandStr.toUtf8();
}

inline QByteArray makeCommand(const QString& command, const quint16& unit) {
  QString unitStr = QString::number(unit);
  QString commandStr = QString("%1%2?\n").arg(command, unitStr);
  return commandStr.toUtf8();
}

inline QByteArray makeReadBackCommand(const QString& command,
                                      const quint16& unit) {
  QString unitStr = QString::number(unit);
  QString commandStr = QString("%1%2O?\n").arg(command, unitStr);
  return commandStr.toUtf8();
}

inline QByteArray makeSwitchOnAllunitsCommand() {
  return switchOnAll.toUtf8();
}

inline QByteArray makeSwitchOffAllunitsCommand() {
  return switchOffAll.toUtf8();
}

inline QByteArray makeSwitchOnUnitCommand(const uint16_t& unit) {
  return makeCommand(switchUnit, unit, 1);
}

inline QByteArray makeSwitchOffUnitCommand(const uint16_t& unit) {
  return makeCommand(switchUnit, unit, 0);
}

inline QByteArray makeSetCurrentLimitCommand(const uint16_t& unit,
                                             const float& value) {
  return makeCommand(current, unit, value);
}

inline QByteArray makeGetCurrentLimitCommand(const uint16_t& unit) {
  return makeCommand(current, unit);
}

inline QByteArray makeGetCurrentValueCommand(const uint16_t& unit) {
  return makeReadBackCommand(current, unit);
}

inline QByteArray makeGetVoltageValueCommand(const uint16_t& unit) {
  return makeReadBackCommand(voltage, unit);
}

inline QByteArray makeSetVcommand(const uint16_t& unit, const float& value) {
  return makeCommand(voltage, unit, value);
}

inline QByteArray makeGetVcommand(const uint16_t& unit) {
  return makeCommand(voltage, unit);
}

inline QByteArray makeGetSwitchStateCommand(const uint16_t& unit) {
  return makeCommand(switchUnit, unit);
}

} // end namespace pwr

#endif // COMMANDS_BUILDER_H
