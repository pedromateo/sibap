// -*- mode: c++; c-basic-offset: 4; c-basic-style: bsd; -*-
/*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU Lesser General Public License as
*   published by the Free Software Foundation; either version 3.0 of
*   the License, or (at your option) any later version.
*
*   This file is part of the SIBAP Prototyping Framework
*   http://pedromateo.github.io/sibap/
*/

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
