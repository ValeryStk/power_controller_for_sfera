QT += testlib core network multimediawidgets widgets printsupport
TARGET = switch_on_off_Tests

INCLUDEPATH += ../../kameya

HEADERS += switch_on_off_UnitTests.h\
           ../../kameya/power_supply_manager.h\
           ../../kameya/json_utils.h\
           ../../kameya/commands_builder.h\

SOURCES += switch_on_off_UnitTests.cpp\
           ../../kameya/power_supply_manager.cpp\
           ../../kameya/json_utils.cpp\

