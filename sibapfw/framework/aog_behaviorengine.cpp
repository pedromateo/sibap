#include "aog_behaviorengine.h"

#include <iostream>
#include <boost/bind.hpp>

namespace csu {
namespace aog {
namespace framework {

template<class T>
BehaviorEngine<T>::BehaviorEngine(BehaviorConfiguration *context){

    std::cout<<"BehaviorEngine::constructor" << std::endl;

    initER = "init__+([a-zA-Z0-9_-]+)";
    stateER = "state_+([a-zA-Z0-9_-]+)+__+([a-zA-Z0-9_-]+)";
    logER = "log__+([a-zA-Z0-9_-]+)";
    defaultER = "([a-zA-Z0-9_-]+)__+([a-zA-Z0-9_-]+)";

    //FIXME TODO Esta ER es temporal a la espera de ver si es posible hacer metodos del tipo
    //estado_e__accion__widget, se utiliza para detectar que se ha definido una funcion correcta
    //para este framework
    definedFuncER = "([a-zA-Z0-9_-]+)__+([a-zA-Z0-9_-]+)";
}

//template<class T>
//void BehaviorEngine<T>::loadScripts(const StringList * scripts)
//{
//    //First load the scripts into the scripting engine, this function returns
//    //the functions loaded
//    FunctionSet * funcs=const_cast<FunctionSet *>(loadScriptsinEngine(scripts));
//
//    //Process the loaded functions
//    processGlobalEnvironmentTable(funcs);
//}

template<class T>
bool BehaviorEngine<T>::callAssertFunction(const std::string &widget)
{
    return this->callFunction("Assert__"+widget);
}

template<class T>
const FunctionSet & BehaviorEngine<T>::functions()
{
    return funcList_;
}

//template<>
//void BehaviorEngine<Lua>::resetEngine()
//{
//    //std::cout << "(LuaBehaviorEngine::resetEngine)" << std::endl;
//    lua_close(L_);
//    //FIXME WATCH esto no es eficiente, ver si no hace falta volver a registrar los widgets, k va a ser k no
//    // y luego ver si afecta mucho el machacar el estado actual para no tener k cerrarlo y volverlo a abrir
//    initialize();
////    L_ = lua_open();
////    luaL_openlibs(L_);
////    luabind::open(L_);  // extra for luabind
////    loadScripts(context_->fileManager->files());
//}
//
//template<>
//void BehaviorEngine<Lua>::initialize()
//{
//    //std::cout << "(LuaBehaviorEngine::constructor)" << std::endl;
//    ///
//    /// Init Lua interpeter
//    ///
//    L_ = lua_open();
//    luaL_openlibs(L_);
//    luabind::open(L_);  // extra for luabind
//
//    /// call the registrar (it needs the lua state pointer)
//    boost::any state_p_obj = L_;
//    context_->elementRegistrar->registerGUIElements(state_p_obj);
//
//    //Loading script files into the scripting engine
//    loadScripts(context_->fileManager->files());
//}
//
//template<>
//const FunctionSet * BehaviorEngine<Lua>::loadScriptsinEngine(const StringList * scripts)
//{
//    std::cout<<"(Lua_BehaviorEngine::loadScripts) Loading scripts inside the LUA engine" << std::endl;
//
//    StringList::const_iterator it=scripts->begin();
//    for (;it!=scripts->end();it++)
//    {
//       // std::cout<<"(Lua_BehaviorEngine::loadScripts) cargando el script "<<*it<<std::endl;
//        int error1 = luaL_dofile(L_,(*it).c_str());
//
//        //FIXME VER ESTE MENSAJE POR DONDE LO SACO O SI CAMBIO ESTO POR EXCEPCIONES
//        if(error1)
//        {
//            std::cerr << "C++: Error al cargar/ejecutar el script: " << lua_tostring(L_, -1) << std::endl;
//            lua_pop(L_, 1);  // pop error message from the stack
//        }
//    }
//    return processGlobals();
//}
//
//template<>
//bool BehaviorEngine<Lua>::callFunction(const std::string & func)
//{
//
//}
//
//template<>
//bool BehaviorEngine<Lua>::callFunction(const FunctionSet & funcList)
//{
//
//}
//
//template<>
//void BehaviorEngine<Lua>::registerFunction(const std::string &state,boost::function <bool(void)> transitionFunc)
//{
//
//}
//
//template<>
//bool BehaviorEngine<Lua>::callAssertFunction(const std::string & widget)
//{
//
//}

template<class T>
bool BehaviorEngine<T>::executeAction(const std::string & action,const std::string & widget)
{
    //std::cout<<"DEBUG: Me ha llegado la accion "<<action<<" procedente del widget "<<widget<<std::endl;
    FunctionSet * fset=&widgetFuncMap_[widget];

    if (fset->size()!=0)
    {
        //FIXME por ahora no filtro si la funcion existe en la tabla de funciones, sino que la paso
        //directamente al motor y que el se encargue de ejecutarla, exista o no
        //asi se ahorran costes de busquedas, ya que los motors suelen tener las tablas
        //globales mejor indexadas y su coste es mejor, o tb se puede asumir el coste de la llamada
        //y el tratamiento del error
        if (this->callAssertFunction(widget))
            this->callFunction(action+"__"+widget);
    }
}


template<class T>
bool BehaviorEngine<T>::makeTransition(const std::string & newState)
{
    std::cout<<"Haciendo la transicion al estado "<<newState<<std::endl;

    FunctionSet::iterator it=stateMap_[newState].begin();

    //FIXME ESTO NO HARÍA FALTA YA QUE EXISTE CALLFUNCTION ACEPTA UN FUNCIONSET
    //FIXME DELETE
    for(;it!=stateMap_[newState].end();it++)
    {
        std::cout<<"Lllamando al a funcion de estado "<<*it<<std::endl;

    }

    this->callFunction(stateMap_[newState]);
    return true;
}

template<class T>
void BehaviorEngine<T>::processGlobalEnvironmentTable(const FunctionSet * funcs)
{
    FunctionSet::const_iterator it=funcs->begin();
    boost::cmatch what;

    for (;it!=funcs->end();it++)
    {
        std::cout<<"Recibida del motor la funcion "<<*it<<std::endl;
        //FIXME TODO hacer una funcion que distinga entre los tipos de funciones
        //y se le pase un functor que sea el que haga todo el trabajo, para no repetir codigo
        if (boost::regex_match((*it).c_str(),what,initER))
        {
            //std::cout<<"Funcion INIT "<<*it<<std::endl;
            initFuncList_.insert(*it);
            widgetFuncMap_[what[1]].insert(*it);
        }else if (boost::regex_match((*it).c_str(),what,stateER))
        {
            /// In what[1] the state
            /// In what[2] the widget

            std::string state="state_"+what[1];
            std::string widget=what[2];

            //std::cout<<"Funcion STATE "<<*it<<std::endl;
            widgetFuncMap_[widget].insert(*it);
            //FIXME FINISH
            if (stateMap_[state].size()==0)
            {
                std::cout<<"Ese estado no esta registrado" << std::endl;
                //First: register a function that make the transition to this state.
                //The function name will be something like state_STATE()

                registerFunction(state,boost::bind(&BehaviorEngine::makeTransition,this,state));

            }

            //Second: store this function inside the functionset of functions of this state
            stateMap_[state].insert(*it);

            this->callFunction(state);

        }else if (boost::regex_match((*it).c_str(),what,logER))
        {
            //FIXME MAYBE NOT NECESARY
            //std::cout<<"Funcion LOG "<<*it<<std::endl;
            logFuncList_.insert(*it);
            widgetFuncMap_[what[1]].insert(*it);

        }else if (boost::regex_match((*it).c_str(),what,defaultER))
        {
            // std::cout<<"what[0] "<<what[0]<<std::endl;
            // std::cout<<"what[1] "<<what[1]<<std::endl;
            // std::cout<<"what[2] "<<what[2]<<std::endl;

            //std::cout<<"Funcion General "<<*it<<std::endl;

            funcList_.insert(*it);
            widgetFuncMap_[what[2]].insert(*it);
        }else{
            continue;
        }
    }
}



}
}
}
