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

#include <string>
#include <list>
#include <set>
#include <map>

#include <boost/regex.hpp>
#include <boost/function.hpp>

#include <framework/aog_forward.h>
#include <framework/aog_behaviorenginebase.h>

namespace csu {
namespace aog {
namespace framework {

class Lua;
class ChaiScript;

typedef std::list<std::string> FunctionList;
typedef std::set<std::string> FunctionSet;

template<class T>
class BehaviorEngine: public BehaviorEngineBase
{
public:
    BehaviorEngine(BehaviorConfiguration* context);
    const FunctionSet & functions();
    void loadScripts(const StringList * scripts);

    /// This function will reset the scripting engine and reload the script files
    void resetEngine();
    void initialize(void);
    bool executeAction(const std::string & action,const std::string & widget);

    //FIXME QUITAR ESTO DE AKI, ESTE CODIGO PERTENECERA A LA MAQUINA DE ESTADOS
    bool makeTransition(const std::string & newState);
    bool callAssertFunction(const std::string & widget);


private:
    void processGlobalEnvironmentTable(const FunctionSet * funcs);


private:
    typedef std::map<std::string,FunctionSet> WidgetFuncMap;
    typedef std::map<std::string,FunctionSet> StateFuncMap;

    BehaviorConfiguration* context_;
    FunctionSet initFuncList_;
    FunctionSet logFuncList_;
    FunctionSet funcList_;
    StateFuncMap stateMap_;
    WidgetFuncMap widgetFuncMap_;
    boost::regex initER;
    boost::regex stateER;
    boost::regex logER;
    boost::regex defaultER;
    boost::regex definedFuncER;
};


//template<class T>
//BehaviorEngine<T>::BehaviorEngine(BehaviorConfigurationPtr context){
//
//}

}
}
}
