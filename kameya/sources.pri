QT += network multimediawidgets widgets printsupport svg
CONFIG += resources_big

HEADERS += \
    $$PWD/config.h \
    $$PWD/graphics_items/power_supply_item.h \
    $$PWD/commands_builder.h\
    $$PWD/MainWindow.h\
    $$PWD/graphics_items\bulbs_item.h\
    $$PWD/icon_generator.h \
    $$PWD/power_supply_manager.h\
    $$PWD/QCustomPlot.h \
    $$PWD/sounder.h \
    $$PWD/json_utils.h\
    $$PWD/qrc_files_restorer.h \
    $$PWD/single_application.h \
    $$PWD/text_log_constants.h

SOURCES += \
    $$PWD/config.cpp \
    $$PWD/graphics_items/power_supply_item.cpp \
    $$PWD/MainWindow.cpp\
    $$PWD/graphics_items\bulbs_item.cpp \
    $$PWD/icon_generator.cpp \
    $$PWD/power_supply_manager.cpp\
    $$PWD/QCustomPlot.cpp \
    $$PWD/sounder.cpp \
    $$PWD/json_utils.cpp\
    $$PWD/qrc_files_restorer.cpp \
    $$PWD/text_log_constants.cpp

RESOURCES += \
    $$PWD/res.qrc

FORMS += \
        $$PWD/MainWindow.ui

INCLUDEPATH += $$PWD
