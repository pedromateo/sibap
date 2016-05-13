#-------------------------------------------------
#
# Project created by QtCreator 2010-03-10T08:55:24
#
#-------------------------------------------------

TARGET = simusaes
TEMPLATE = app

QT += widgets


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


INCLUDEPATH += ../sibapfw
LIBS += -L../sibapfw -lsibapfw

LIBS += -lboost_system -lboost_program_options

### some lines here should be changed for compilation in your computer >>>
LIBS += -L/opt/boost/boost_1_54_0/lib/
#INCLUDEPATH += /opt/boost/boost_1_54_0/include/
### some lines here should be changed for compilation in your computer <<<

DISTFILES += \
    behavior/alternative_design.lua \
    behavior/MainWindow/calibration.lua \
    behavior/MainWindow/initialization.lua



