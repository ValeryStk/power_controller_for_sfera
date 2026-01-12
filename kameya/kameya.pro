QT += network multimediawidgets widgets printsupport svg
CONFIG += c++17
TARGET = power_controller
CONFIG += resources_big
RC_FILE = recource.rc
RESOURCES += \
    res.qrc
include(sources.pri)




