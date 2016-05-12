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

#ifndef QTLUA_ELEMENTREGISTRAR_H
#define QTLUA_ELEMENTREGISTRAR_H


#include <framework/aog_genericelementregistrar.h>

#include <lua5.1/lua.hpp>

namespace csu {
namespace aog {
namespace gui_vlang {
namespace qt_lua {

using namespace csu::aog::framework;

class QtLua_ElementRegistrar : public GenericElementRegistrar
{
public:
    ///
    /// Constructor
    /// @param context is the verification context
    ///

    QtLua_ElementRegistrar(BehaviorConfiguration* context);
    virtual ~QtLua_ElementRegistrar();

    virtual boost::any registerGUIElements(boost::any& object);

    void windowCreate(const std::string & windowName);
    void windowDelete(const std::string & windowName);


private:
    virtual boost::any updateGUIElementRegister();
private:
    lua_State * L_;
    std::set<std::string> tables_;
};


}
}
}
}
#endif // QTLUA_ELEMENTREGISTRAR_H
