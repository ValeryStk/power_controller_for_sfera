QT += network multimediawidgets widgets printsupport svg
CONFIG += resources_big

HEADERS += \
    $$PWD/config.h \
    $$PWD/graphics_items/power_supply_item.h \
    $$PWD/commands_builder.h\
    $$PWD/MainWindow.h\
    $$PWD/graphics_items\OpticTable.h\
    $$PWD/icon_generator.h \
    $$PWD/power_supply_manager.h\
    $$PWD/QCustomPlot.h \
    $$PWD/QrcFilesRestorer.h \
    $$PWD/Sounder.h \
    $$PWD/Types.h\
    $$PWD/json_utils.h\
    $$PWD/text_log_constants.h

SOURCES += \
    $$PWD/config.cpp \
    $$PWD/graphics_items/power_supply_item.cpp \
    $$PWD/MainWindow.cpp\
    $$PWD/graphics_items\OpticTable.cpp \
    $$PWD/icon_generator.cpp \
    $$PWD/power_supply_manager.cpp\
    $$PWD/QCustomPlot.cpp \
    $$PWD/QrcFilesRestorer.cpp \
    $$PWD/Sounder.cpp \
    $$PWD/json_utils.cpp\
    $$PWD/text_log_constants.cpp

RESOURCES += \
    $$PWD/res.qrc

FORMS += \
        $$PWD/MainWindow.ui

INCLUDEPATH += $$PWD
