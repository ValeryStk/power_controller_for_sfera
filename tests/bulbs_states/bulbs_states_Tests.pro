QT += testlib
include(../../kameya/sources.pri)
INCLUDEPATH += ../../kameya
TARGET = bulbs_states_Tests
HEADERS += bulbs_states_UnitTests.h\
    mock_power_server.h

SOURCES += bulbs_states_UnitTests.cpp\
    mock_power_server.cpp

RESOURCES += \
    mock_res.qrc
