TARGET = sibapfw
TEMPLATE = lib

CONFIG += qt
QT += gui widgets



SOURCES += \
#framework
./framework/aog_behaviorconfiguration.cpp \
./framework/aog_executionresult.cpp \
./framework/aog_genericeventabstractor.cpp \
./framework/aog_logmanager.cpp \
./framework/aog_behaviorengine.cpp \
./framework/aog_genericbehaviorengine.cpp \
./framework/aog_genericfilemanager.cpp \
./framework/aog_statemachine.cpp \
./framework/aog_behaviorlayer.cpp \
./framework/aog_genericelementregistrar.cpp \
./framework/aog_genericfunctioncaller.cpp \
#log
./config/log_config_internal.cpp \
#gui implementation
./imps/gui/qt/qt_eventabstractor.cpp \
#language implementation
./imps/vlang/lua/lua_behaviorengine.cpp \
./imps/vlang/lua/lua_functioncaller.cpp \
#gui+language implementation
./imps/gui_vlang/qtlua_adapters.cpp \
./imps/gui_vlang/qtlua_behaviorlayer.cpp \
./imps/gui_vlang/qtlua_elementregistrar.cpp



HEADERS += \
#framework
./framework/aog_behaviorconfiguration.h \
./framework/aog_eventobserver.h \
./framework/aog_genericbehaviorengine.h \
./framework/aog_logmanager.h \
./framework/aog_behaviorenginebase.h \
./framework/aog_executionresult.h \
./framework/aog_genericelementregistrar.h \
./framework/aog_statemachine.h \
./framework/aog_behaviorengine.h \
./framework/aog_fileobservable.h \
./framework/aog_genericeventabstractor.h \
./framework/aog_behaviorlayer.h \
./framework/aog_fileobserver.h \
./framework/aog_genericfilemanager.h \
./framework/aog_eventobservable.h \
./framework/aog_forward.h \
./framework/aog_genericfunctioncaller.h \
#log
./config/log_config_internal.h \
#gui implementation
./imps/gui/qt/qt_eventabstractor.h \
#language implementation
./imps/vlang/lua/lua_behaviorengine.h \
./imps/vlang/lua/lua_functioncaller.h \
#gui+language implementation
./imps/gui_vlang/qtlua_adapters.h \
./imps/gui_vlang/qtlua_behaviorlayer.h \
./imps/gui_vlang/qtlua_elementregistrar.h


### maybe you need some includes >>>
INCLUDEPATH += /usr/include/lua5.1/ #\
#    /usr/local/include/luabind/

LIBS += -L/opt/boost/boost_1_54_0/lib/
#INCLUDEPATH += /opt/boost/boost_1_54_0/include/
### maybe you need some includes <<<

LIBS += -llua5.1 -lluabindd \
    -L/usr/local/lib/

LIBS += -lboost_thread \
    -lboost_system \
    -lboost_filesystem \
    -lboost_date_time \
    -lboost_regex \
    -lboost_program_options
