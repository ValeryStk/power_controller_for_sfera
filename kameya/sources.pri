HEADERS += \
    $$PWD/config.h \
    $$PWD/graphics_items/power_supply_item.h \
    $$PWD/commands_builder.h\
    $$PWD/MainWindow.h\
    $$PWD/graphics_items\OpticTable.h\
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
    $$PWD/power_supply_manager.cpp\
    $$PWD/QCustomPlot.cpp \
    $$PWD/QrcFilesRestorer.cpp \
    $$PWD/Sounder.cpp \
    $$PWD/main.cpp\
    $$PWD/json_utils.cpp\
    $$PWD/text_log_constants.cpp


FORMS += \
        $$PWD/MainWindow.ui

INCLUDEPATH += $$PWD
