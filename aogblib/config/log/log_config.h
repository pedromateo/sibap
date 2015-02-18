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
 */
#ifndef LOG_CONFIG_H
#define LOG_CONFIG_H


#include <iostream>

///
/// 1. Include here the log library (*.hpp)
///
#include <config/log/log.hpp>

namespace csu {
namespace base {
namespace conf {
namespace log {

///
/// 2. Declare here the logs as extern variables.
///
/// The declaration of a log is composed of:
/// - a boolean parameter which indicates that
///   the log is enabled or disabled.
/// - the name of the log (op)
/// - the stream the log is associated to (op)
///
/// The typedefs are not necessary.
///

///
/// global logs
///


/// Debug error
typedef csu::base::log::logger<true> error_log_type;
extern error_log_type error;

/*
/// Debug misc
typedef csu::base::log::logger<true> misc_log_type;
extern misc_log_type misc;

/// Debug comm
typedef csu::base::log::logger<true> comm_log_type;
extern comm_log_type comm;

/// Debug gui
typedef csu::base::log::logger<true> gui_log_type;
extern gui_log_type gui;

/// Debug time
typedef csu::base::log::logger<true> time_log_type;
extern time_log_type time;

/// Debug control
typedef csu::base::log::logger<true> control_log_type;
extern control_log_type control;
*/

}//namespace csu
}//namespace base
}//namespace conf
}//namespace log

// Convenient alias
//namespace _log = csu::base::conf::log; // commented because of ambiguity problem

#endif // LOG_CONFIG_H
