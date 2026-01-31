QT += core network
CONFIG += c++17 console
CONFIG -= windows
TARGET = mock_power_servers

include(../pathes.pri)
KAMEYA_DIR =  $$ROOT_DIR/kameya

SOURCES += $$PWD/mock_power_server.cpp\
$$KAMEYA_DIR/config.cpp\
$$KAMEYA_DIR/json_utils.cpp\

HEADERS += $$PWD/mock_power_server.h\
$$KAMEYA_DIR/config.h\
$$KAMEYA_DIR/json_utils.h\

INCLUDEPATH += $$KAMEYA_DIR\

SOURCES += \
    $$PWD/main.cpp\
