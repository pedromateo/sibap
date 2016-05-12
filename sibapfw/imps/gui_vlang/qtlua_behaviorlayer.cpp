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

#include "qtlua_behaviorlayer.h"

#include <config/log_config_internal.h>

#include <imps/gui_vlang/qtlua_elementregistrar.h>
#include <imps/gui/qt/qt_eventabstractor.h>
#include <imps/vlang/lua/lua_behaviorengine.h>


namespace csu {
namespace aog {
namespace gui_vlang {
namespace qt_lua {

using namespace csu::aog::config::log;

QtLua_BehaviorLayer::QtLua_BehaviorLayer(BehaviorConfiguration *context):BehaviorLayer(context){}

void QtLua_BehaviorLayer::createElementRegistrar(void)
{
    _log::debug <<"QtLua_VerificationLayer: Creating the Lua Rule Manager" << std::endl;
    context_->elementRegistrar.reset(
                new csu::aog::gui_vlang::qt_lua::QtLua_ElementRegistrar(context_) );
}

void QtLua_BehaviorLayer::createEventAbstractor(void)
{
    _log::debug <<"QtLua_VerificationLayer: Creating the Event Abstractor" << std::endl;
    context_->eventAbstractor.reset(
                new csu::aog::gui::qt::QtEventAbstractor(context_));
}

void QtLua_BehaviorLayer::createBehaviorEngine(void)
{
    _log::debug <<"QtLua_VerificationLayer: Creating the Aspect Engine" << std::endl;
    context_->aspectEngine.reset(
                new csu::aog::vlang::lua::Lua_BehaviorEngine(context_));
}

}
}
}
}
