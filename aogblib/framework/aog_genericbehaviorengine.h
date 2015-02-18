// -*- mode: c++; c-basic-offset: 4; c-basic-style: bsd; -*-
/*
*   This program is free software; you can redistribute it and/or
*   modify
*   it under the terms of the GNU Lesser General Public License as
*   published by the Free Software Foundation; either version 3.0 of
*   the License, or (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU Lesser General Public License for more details.
*
*   You should have received a copy of the GNU Lesser General Public
*   License along with this library; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
*   02111-1307 USA
*
*   This file is part of the Aspect-Oriented Behavior Framework,
*   https://sourceforge.net/projects/aobf/
*
*/

#ifndef AOG_GENERICBehaviorEngine_H
#define AOG_GENERICBehaviorEngine_H

#include <framework/aog_forward.h>
#include <framework/aog_statemachine.h>
#include <framework/aog_behaviorconfiguration.h>

#include <boost/regex.hpp>
#include <boost/function.hpp>

//log
#include <config/log_config_internal.h>

#include <map>

#define INTERNALLOG "__log"
#define INTERNALFILELOG "__flog"


namespace csu {
namespace aog {
namespace framework {

class GenericBehaviorEngine
{
public:
    GenericBehaviorEngine(BehaviorConfiguration* context);
    const FunctionSet & functions();
    void loadScripts(const StringList * scripts);

    void initializeEngine();

    /// This function will reset the scripting engine and reload the script files
    virtual void resetEngine()=0;

    bool executeAction(const std::string & action,const std::string & widget);
    bool executeFunction(const std::string & function);
    bool executeFunction(const FunctionSet & funcList);


protected:
    /// This function will initialize the script engine
    virtual void initialize(void)=0;

    virtual const FunctionSet * loadScriptsinEngine(const StringList * scripts)=0;

    virtual bool callFunction(const std::string & func)=0;
    virtual bool callFunction(const FunctionSet & funcList)=0;
    virtual bool functionExists(const std::string & func)=0;

    virtual void deleteFunction(const std::string & func)=0;
    virtual void registerFunction(const std::string &state,boost::function <bool(void)> transitionFunc)=0;

    virtual bool callAssertFunction(const std::string & widget);

    virtual void registerLogGlobalFunc(const std::string &funcName, boost::function<void (std::string)>fp)=0;
    virtual void registerLogSpecificFunc(const std::string &funcName, boost::function<void (std::string,std::string)>fp)=0;

private:
    void processGlobalEnvironmentTable(const FunctionSet * funcs);

protected:
    typedef std::map<std::string,FunctionSet> WidgetFuncMap;

    BehaviorConfiguration* context_;
    FunctionSet globalFuncList_;
    WidgetFuncMap widgetFuncMap_;

    boost::regex initER;
    boost::regex stateER;
    boost::regex logwidgetER;
    boost::regex logactionER;
    boost::regex defaultER;
    boost::regex definedFuncER;
    boost::regex stateactionER;

private:
    StateMachine * stmachine_;
    std::string separator;
};

}
}
}

#endif // AOG_GENERICBehaviorEngine_H
