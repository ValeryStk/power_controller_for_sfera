QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app
HEADERS +=  /commands_builder.h\
            ../../kameya/CommandsBuilder.h

SOURCES +=  tst_commandsbuildertest.cpp\
            commands_builder.cc
