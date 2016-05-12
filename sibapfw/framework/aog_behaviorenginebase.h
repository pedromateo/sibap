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

#include <framework/aog_forward.h>

#include <set>
#include <boost/function.hpp>

namespace csu {
namespace aog {
namespace framework {

//FIXME Si funciona, quitar esto de aqui y ponerlo en forward
typedef std::list<std::string> FunctionList;
typedef std::set<std::string> FunctionSet;

class BehaviorEngineBase
{
 virtual const FunctionSet * loadScriptsinEngine(const StringList * scripts)=0;
 virtual bool callFunction(const std::string & func)=0;
 virtual bool callFunction(const FunctionSet & funcList)=0;
 virtual void registerFunction(const std::string &state,boost::function <bool(void)> transitionFunc)=0;

};

}
}
}
