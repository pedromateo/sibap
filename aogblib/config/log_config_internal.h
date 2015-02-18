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

#ifndef LOG_CONFIG_INTERNAL_H
#define LOG_CONFIG_INTERNAL_H

#include <config/log/log_config.h>


namespace csu {
namespace aog {
namespace config {
namespace log {

// Convenient alias
namespace _log = csu::aog::config::log;

///
/// aog logs
///
/// gui ..... for everything related to user interface
/// behav ... for everything related to behavior layer or engine
/// script .. for everything related to script files
/// debug ... for every other things - will be removed in release version

typedef csu::base::log::logger<true> debug_log_type;
extern debug_log_type debug;

typedef csu::base::log::logger<true> gui_log_type;
extern gui_log_type gui;

typedef csu::base::log::logger<true> behav_log_type;
extern behav_log_type behav;

typedef csu::base::log::logger<true> script_log_type;
extern script_log_type script;

///
/// Internal log configuration
///
class LogConfigInternal
{
public:
    /// initialization method
    static void initialize();
};



}
}
}
}
#endif // LOG_CONFIG_INTERNAL_H
