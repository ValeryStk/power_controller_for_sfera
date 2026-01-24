QT += testlib core widgets
TARGET = json_config_to_struct_Tests

KAMEYA_DIR = ../../kameya

INCLUDEPATH += $$KAMEYA_DIR
include($$KAMEYA_DIR/restore_res.pri)

HEADERS += json_config_to_struct_UnitTests.h\
           $$KAMEYA_DIR/config.h\
           $$KAMEYA_DIR/json_utils.h\
           $$KAMEYA_DIR/qrc_files_restorer.h \

SOURCES += json_config_to_struct_UnitTests.cpp\
           $$KAMEYA_DIR/config.cpp\
           $$KAMEYA_DIR/json_utils.cpp\
           $$KAMEYA_DIR/qrc_files_restorer.cpp \

RESOURCES += \
    test_res.qrc

