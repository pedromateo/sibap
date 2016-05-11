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

#include "log_config_internal.h"

namespace csu {
namespace aog {
namespace config {
namespace log {

    ///
    /// aog logs
    ///


    debug_log_type debug("debug");
    gui_log_type gui("gui");
    behav_log_type behav("behav");
    script_log_type script("script");

    ///
    /// Internal log configuration
    ///
    void LogConfigInternal::initialize()
    {
        ///
        /// csu_lib logs
        ///
        ///

        //we'll use
//        _log::misc << csu::base::log::on;
//        _log::gui << csu::base::log::on;
//        //we won't use
//        _log::comm << csu::base::log::off;
//        _log::time << csu::base::log::off;
//        _log::control << csu::base::log::off;
//
        ///
        /// internal logs
        ///

        _log::debug.add_stream (std::cout);
        _log::debug << csu::base::log::on;

        _log::gui.add_stream (std::cout);
        _log::gui << csu::base::log::off;

        _log::behav.add_stream (std::cout);
        _log::behav << csu::base::log::off;

        _log::script.add_stream (std::cout);
        _log::script << csu::base::log::on;
    }

}
}
}
}

