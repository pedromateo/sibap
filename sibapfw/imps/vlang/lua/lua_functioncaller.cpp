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

#include "lua_functioncaller.h"
#include <luabind/luabind.hpp>
#include <boost/lexical_cast.hpp>


#include <framework/aog_definitions.h>

//#################################################################
//###  TROZO DE CODIGO DE PRUEBA DE LLAMADAS A FUNCIONES LUA  #####
//#################################################################
#include <framework/aog_behaviorconfiguration.h>
#include <framework/aog_genericelementregistrar.h>


#include <iostream>
#include <boost/any.hpp>

namespace csu {
namespace aog {
namespace vlang {
namespace lua {

using namespace csu::aog::framework;

LuaFunctionCaller::LuaFunctionCaller(BehaviorConfiguration *context):GenericFunctionCaller(context)
{
    initER = BF_INIT "" BF_DSEP "+([a-zA-Z0-9_-]+)";
    stateER = BF_STATE "+([a-zA-Z0-9_-]+)+" BF_DSEP "+([a-zA-Z0-9_-]+)";
    logER = BF_LOG "" BF_DSEP "+([a-zA-Z0-9_-]+)";
    defaultER = "([a-zA-Z0-9_-]+)" BF_DSEP "+([a-zA-Z0-9_-]+)";

    std::cout<<"LUAFunctionCaller::constructor" << std::endl;
}

void LuaFunctionCaller::initialize()
{
    ///
    /// Init Lua interpeter
    ///
    L_ = lua_open();
    luaL_openlibs(L_);
    luabind::open(L_);  // extra for luabind

    /// call the registrar (it needs the lua state pointer)
    boost::any state_p_obj = L_;
    boost::any result = context_->elementRegistrar->registerGUIElements(state_p_obj);

    std::cout << "(Lua_FunctionCaller::constructor) Finished." << std::endl;
    processGlobals();
}

const FunctionList & LuaFunctionCaller::functions()
{
    return funcList_;
}

void LuaFunctionCaller::luaState(lua_State * state)
{
    L_=state;
}

const lua_State * LuaFunctionCaller::luaState(void)
{
    return L_;
}

//FIXME DELETE, JUST FOR TESTING PURPOSES
//int geterror( lua_State* L ) {
//
//    std::cout<<"ERROR HANDLER" << std::endl;
//        return 1;
//
//}

bool LuaFunctionCaller::callFunction(const std::string & func)
{
    //We can set an error callback to be called when an error occurs inside a luabind::call_function function
    //luabind::set_pcall_callback( &geterror );
    try
    {
        luabind::call_function<void>(L_, func.c_str(), 0, 0);
        return true;
    }catch(const luabind::error & e){
        //if std::exception is used as the exception type only a "lua runtime error" message will be catched
        //std::cout<<"Generada excepcion "<<e.what()<<std::endl;

        //if we use a luabind::error to store the exception we will have more information about the real reason
        //of the error
        //NOTICE: althought the error handler is set, this message would be prompted
        luabind::object error_msg(luabind::from_stack(e.state(), -1));
        std::cout << error_msg << std::endl;
        return false;
    }
}

void LuaFunctionCaller::processGlobals()
{
    /*

      EN PRINCIPIO ESTO YA NO VALE PARA NADA
        Para encontrar las funciones es necesario obtener todas las tablas de se han debido crear
        en la fase de registro de widgets y donde cada tabla se corresponde con un dialogo. Dentro de cada
        Dialogo iterar para encontrar las funciones

       // Ejemplo, primero obtenemos la tabla asociada a la ventana principal MainWindow
        luabind::object MainWindow = luabind::globals(L)["MainWindow"];
        //Comprobar que existe la tabla
        if(MainWindow && luabind::type(MainWindow) != LUA_TNIL)
        {
            //Obtenemos la funcion examplefunc de la tabla MainWindow
            luabind::object func = MainWindow["examplefunc"];
            //Comprobamos que es una funcion
            if(func && luabind::type(func) == LUA_TFUNCTION)
            {
                //Hacemos algo
            }
        }
      */


    for (luabind::iterator i(luabind::globals(L_)), end; i != end; ++i)
    {
            std::string aux=boost::lexical_cast<std::string>(i.key());

            if (luabind::type(*i) == LUA_TFUNCTION)
            {

                boost::cmatch what;

                //FIXME TODO hacer una funcion que distinga entre los tipos de funciones
                //y se le pase un functor que sea el que haga todo el trabajo, para no repetir codigo
                if (boost::regex_match(aux.c_str(),what,initER))
                {
                    std::cout<<"Funcion INIT "<<aux<<std::endl;
                    initFuncList_.push_back(aux);
                    widgetFuncMap_[what[1]].insert(aux);
                }else if (boost::regex_match(aux.c_str(),what,stateER))
                {
                    std::cout<<"Funcion STATE "<<aux<<std::endl;
                    widgetFuncMap_[what[2]].insert(aux);
                    //FIXME FINISH

                    //In what[1] the state
                    //In what[2] the widget

                }else if (boost::regex_match(aux.c_str(),what,logER))
                {
                    //MAYBE NOT NECESARY
                    std::cout<<"Funcion LOG "<<aux<<std::endl;
                    logFuncList_.push_back(aux);
                    widgetFuncMap_[what[1]].insert(aux);

                }else if (boost::regex_match(aux.c_str(),what,defaultER))
                {
                    //MAYBE NOT NECESARY
                    std::cout<<"Funcion General "<<aux<<std::endl;
                    funcList_.push_back(aux);
                    widgetFuncMap_[what[2]].insert(aux);
//                    std::cout<<"what[0] "<<what[0]<<std::endl;
//                    std::cout<<"what[1] "<<what[1]<<std::endl;
//                    std::cout<<"what[2] "<<what[2]<<std::endl;
                }else{
                    continue;
                }


            }
            //std::cout<<std::endl;

    }
}

//FIXME DELETE
//LLAMAR FUNCIONES
/*
luabind::object Print2000 = luabind::globals(L)["Print2000"];
if(Print2000 && luabind::type(Print2000) != LUA_TNIL)
{
   luabind::object CallBack = Print2000["mObject"];
   if(CallBack && luabind::type(CallBack) == LUA_TFUNCTION)
   {
       // pass in the 'Self'
       luabind::call_function<void>(CallBack, boost::ref(Print2000));
   }
*/

}
}
}
}
