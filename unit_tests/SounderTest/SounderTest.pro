QT += testlib gui multimediawidgets

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app
INCLUDEPATH +=../../kameya/



HEADERS += ../../kameya/Sounder.h\

SOURCES +=  tst_soundertest.cpp\
../../kameya/Sounder.cpp

RESOURCES += \
    ../../kameya/res.qrc
