QT += core network widgets
CONFIG += c++17
TARGET = mock_power_servers

include(../pathes.pri)

SOURCES += $$MOCK_SERVER_PATH/mock_power_server.cpp\
MainWindow.cpp\

HEADERS += $$MOCK_SERVER_PATH/mock_power_server.h\
MainWindow.h\

INCLUDEPATH += $$MOCK_SERVER_PATH

SOURCES += \
    $$PWD/main.cpp\

