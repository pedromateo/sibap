#-------------------------------------------------
#
# Project created by QtCreator 2010-03-10T08:55:24
#
#-------------------------------------------------

TARGET = simusaes
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += -L../aogblib -laogb
INCLUDEPATH += ../aogblib

OTHER_FILES += behavior/initialization.lua \
    behavior/proto_behavior.lua \
    behavior/alternative_design.lua \
    layer.config

LIBS += -lboost_system -lboost_program_options
