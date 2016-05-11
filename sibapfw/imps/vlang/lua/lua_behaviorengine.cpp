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

#include "lua_behaviorengine.h"

#include <framework/aog_behaviorconfiguration.h>
#include <config/log_config_internal.h>

#include <luabind/luabind.hpp>
#include <luabind/tag_function.hpp>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

namespace csu {
namespace aog {
namespace vlang {
namespace lua {

using namespace csu::aog::framework;
using namespace csu::aog::config::log;

Lua_BehaviorEngine::Lua_BehaviorEngine(BehaviorConfiguration *context):GenericBehaviorEngine(context)
{}



void Lua_BehaviorEngine::initialize()
{
    _log::debug << "(LuaBehaviorEngine::initialize)" << std::endl;
    ///
    /// Init Lua interpeter
    ///
    L_ = lua_open();
    luaL_openlibs(L_);
    luabind::open(L_);  // extra for luabind

    /// call the registrar (it needs the lua state pointer)
    boost::any state_p_obj = L_;
    context_->elementRegistrar->registerGUIElements(state_p_obj);

    //Loading script files into the scripting engine
    loadScripts(context_->fileManager->files());
}

void Lua_BehaviorEngine::resetEngine()
{
    _log::debug<< "(LuaBehaviorEngine::resetEngine)" << std::endl;
    lua_close(L_);

    initialize();
}


bool Lua_BehaviorEngine::functionExists(const std::string & func)
{
    luabind::object obfunc = luabind::globals(L_)[func];
     if(obfunc && luabind::type(obfunc) == LUA_TFUNCTION)
         return true;

     return false;
 }


bool Lua_BehaviorEngine::callFunction(const std::string & func)
{
    //_log::debug << "(Lua_BehaviorEngine::callFunction)" << std::endl;
    try
    {
        //Checking if the function is registered in the global table
        //I dont know why but this is necessary!!
        luabind::object obfunc = luabind::globals(L_)[func];

        if (functionExists(func))
        {
            _log::debug << "(Lua_BehaviorEngine::callFunction) Calling Function '"<<func<<"'" << std::endl;
            return luabind::call_function<bool>(L_, func.c_str());
        }

        return false;
    }
    catch(luabind::error & e){

        /// if std::exception is used as the exception type only a "lua runtime error" message will be catched

        /// if we use a luabind::error to store the exception we will have more information about the real reason
        /// of the error
        /// NOTICE: althought the error handler is set, this message would be prompted
        luabind::object error_msg(luabind::from_stack(e.state(), -1));
        _log::debug << "(Lua_BehaviorEngine::callFunction) ERROR: "<<error_msg << std::endl;
        context_->logManager->doLog("ERROR: "+luabind::object_cast<std::string>(error_msg));
        return false;
    }
    catch(luabind::cast_failed e) {
        return false;
    }
}


bool Lua_BehaviorEngine::callFunction(const FunctionSet & funcList)
{
    FunctionSet::const_iterator it= funcList.begin();

    for (;it!=funcList.end();it++)
    {
        callFunction(*it);
    }
    return true;
}

const FunctionSet * Lua_BehaviorEngine::loadScriptsinEngine(const StringList * scripts)
{
    _log::debug << "(Lua_BehaviorEngine::loadScripts) Loading scripts inside the LUA engine" << std::endl;

    StringList::const_iterator it=scripts->begin();
    for (;it!=scripts->end();it++)
    {
        std::cout<<"(Lua_BehaviorEngine::loadScripts) Loading script "<<*it<<std::endl;
        int error1 = luaL_dofile(L_,(*it).c_str());

        if(error1)
        {
            _log::debug << "C++: Error al cargar/ejecutar el script: " << lua_tostring(L_, -1) << std::endl;
            context_->logManager->doLog("ERROR: "+ std::string(lua_tostring(L_, -1)));
            lua_pop(L_, 1);  // pop error message from the stack
        }
    }
    return processGlobals();
}

const FunctionSet * Lua_BehaviorEngine::processGlobals()
{
    FunctionSet * funcs=new FunctionSet();
    boost::cmatch what;

    for (luabind::iterator i(luabind::globals(L_)), end; i != end; ++i)
    {
        std::string aux=boost::lexical_cast<std::string>(i.key());

        if (luabind::type(*i) == LUA_TFUNCTION)
        {
            if (boost::regex_match(aux.c_str(),what,definedFuncER))
                funcs->insert(aux);

        }
    }
    return funcs;
}


void Lua_BehaviorEngine::registerLogGlobalFunc(const std::string &funcName, boost::function<void (std::string)>fp)
{
    _log::debug << "(Lua_BehaviorEngine::registerLogGlobalFunc) Registering Log Global Function '" << std::endl;

    luabind::module(L_)
    [
          luabind::def(funcName.c_str(),luabind::tag_function<void (std::string)>(boost::function<void (std::string)> (fp)))
    ];
}

void Lua_BehaviorEngine::registerLogSpecificFunc(const std::string &funcName, boost::function<void (std::string,std::string)>fp)
{
    _log::debug << "(Lua_BehaviorEngine::registerLogSpecificFunc) Registering Log Internal Functions '" << std::endl;


    luabind::module(L_)
    [
          luabind::def(funcName.c_str(),luabind::tag_function<void (std::string,std::string)>(fp))
    ];


}

void Lua_BehaviorEngine::deleteFunction(const std::string & func)
{
    luabind::globals(L_)[func]=luabind::nil;
}

void Lua_BehaviorEngine::registerFunction(const std::string &state,boost::function <bool(void)>  transitionFunc)
{

    _log::debug << "(Lua_BehaviorEngine::registerFunction) Registering the new state '"<<state<<"'" << std::endl;

    luabind::module(L_)
    [
            luabind::def(state.c_str(),luabind::tag_function<bool(void)>(transitionFunc))
    ];

}


}
}
}
}

