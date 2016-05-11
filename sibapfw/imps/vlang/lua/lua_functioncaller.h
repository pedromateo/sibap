#ifndef LUA_FUNCTIONCALLER_H
#define LUA_FUNCTIONCALLER_H

#include <lua5.1/lua.hpp>
#include <list>
#include <map>
#include <set>
#include <string>
#include <boost/regex.hpp>

#include <framework/aog_forward.h>

#include <framework/aog_genericfunctioncaller.h>

//#################################################################
//###  TROZO DE CODIGO DE PRUEBA DE LLAMADAS A FUNCIONES LUA  #####
//#################################################################

namespace csu {
namespace aog {
namespace vlang {
namespace lua {

typedef std::list<std::string> FunctionList;
typedef std::set<std::string> FunctionSet;
typedef std::map<std::string,FunctionSet> WidgetFuncMap;

class LuaFunctionCaller: public csu::aog::framework::GenericFunctionCaller
{
public:
    LuaFunctionCaller(csu::aog::framework::BehaviorConfiguration* context);
    void luaState(lua_State * state);
    const lua_State * luaState(void);
    void processGlobals();
    const FunctionList & functions();
    bool callFunction(const std::string & func);

    void initialize(void);

private:
    lua_State * L_;

private:
    FunctionList initFuncList_;
    FunctionList logFuncList_;
    FunctionList funcList_;
    WidgetFuncMap widgetFuncMap_;
    boost::regex initER;
    boost::regex stateER;
    boost::regex logER;
    boost::regex defaultER;

};

}
}
}
}

#endif // LUA_FUNCTIONCALLER_H
