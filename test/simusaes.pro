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


INCLUDEPATH += ../sibapfw
LIBS += -L../sibapfw -lsibapfw

OTHER_FILES += behavior/initialization.lua \
    behavior/proto_behavior.lua \
    behavior/alternative_design.lua \
    layer.config

### maybe you need some includes >>>
LIBS += -L/opt/boost/boost_1_54_0/lib/
INCLUDEPATH += /opt/boost/boost_1_54_0/include/
### maybe you need some includes <<<

LIBS += -lboost_system -lboost_program_options
