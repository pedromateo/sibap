#include <QtGui/QApplication>
#include "mainwindow.h"

#include <imps/gui_vlang/qtlua_behaviorlayer.h>
#include <framework/aog_behaviorconfiguration.h>

using namespace csu::aog::framework;
using namespace csu::aog::gui_vlang::qt_lua;

int main(int argc, char *argv[])
{
    QApplication qapp(argc, argv);
    MainWindow w;
    w.show();

    ///
    /// behavior layer configuration

    // Alternative 1
    //BehaviorConfiguration bconf("../layer.config");

    // Alternative 2
    //BehaviorConfiguration bconf;
    //bconf.loadConfigFile("layer.config");

    // Alternative 3
    BehaviorConfiguration bconf;
    bconf.updateFileTime(10)
         .logFormat("[%tm] %wi Event:%en :: State: %cs :: Assertion: %ar :: Function Called: %fc :: Result: %fr")
         .addLogOutputFile("/tmp/behaviorlog.log")
         .addScriptPath("../behavior/");

    // You can also configure if the log info goes to the screen.
    // Available parameters are on,off
    // StandardOutput=on
    // StandardError=off

    ///
    /// behavior layer deployment

    QtLua_BehaviorLayer blayer(&bconf);
    blayer.init();

    ///

    return qapp.exec();
}
