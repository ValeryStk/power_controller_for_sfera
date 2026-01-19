QT += testlib core widgets
TARGET = json_config_to_struct_Tests

include(../../kameya/sources.pri)
KAMEYA_DIR = ../../kameya

INCLUDEPATH += $$KAMEYA_DIR

HEADERS += json_config_to_struct_UnitTests.h\

SOURCES += json_config_to_struct_UnitTests.cpp\
