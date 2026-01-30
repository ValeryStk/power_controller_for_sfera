QT += core network widgets
CONFIG += c++17
TARGET = mock_power_servers

include(../pathes.pri)
KAMEYA_DIR =  $$ROOT_DIR/kameya

SOURCES += $$MOCK_SERVER_PATH/mock_power_server.cpp\
MainWindow.cpp\
$$KAMEYA_DIR/config.cpp\
$$KAMEYA_DIR/json_utils.cpp\

HEADERS += $$MOCK_SERVER_PATH/mock_power_server.h\
MainWindow.h\
$$KAMEYA_DIR/config.h\
$$KAMEYA_DIR/json_utils.h\

INCLUDEPATH += $$MOCK_SERVER_PATH\
               $$KAMEYA_DIR\

SOURCES += \
    $$PWD/main.cpp\

