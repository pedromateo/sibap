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
    context_->elementRegistrar.reset(new csu::aog::gui_vlang::qt_lua::QtLua_ElementRegistrar(context_) );
}

void QtLua_BehaviorLayer::createEventAbstractor(void)
{
    _log::debug <<"QtLua_VerificationLayer: Creating the Event Abstractor" << std::endl;
    context_->eventAbstractor.reset(new csu::aog::gui::qt::QtEventAbstractor(context_));
}

void QtLua_BehaviorLayer::createBehaviorEngine(void)
{
    _log::debug <<"QtLua_VerificationLayer: Creating the Aspect Engine" << std::endl;
    context_->aspectEngine.reset(new csu::aog::vlang::lua::Lua_BehaviorEngine(context_));
}

}
}
}
}
