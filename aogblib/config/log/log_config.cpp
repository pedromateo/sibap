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

///
/// include the log and the config file
///

#include "log_config.h"

///
/// 4. Create the logs declared in the log
/// config file and initialize them.
///
/// The parameter is the identifier of the
/// log.
///
/// Note that this part has to be done here
/// because the logs are extern variables.
///
namespace csu {
   namespace base {
      namespace conf {
         namespace log {

            ///
            /// global logs
            ///

            error_log_type error("error",std::cerr,false);
            /*misc_log_type misc("misc",std::cout,false);
            comm_log_type comm("comm",std::cout,false);
            gui_log_type gui("gui",std::cout,false);
            time_log_type time("time",std::cout,false);
            control_log_type control("control",std::cout,false);*/

         }//namespace csu
      }//namespace base
   }//namespace conf
}//namespace log


#if 0

int main(/*int argc, char *argv[]*/)
{
   ///
   /// 5. Initialize here the logs (e.g. add
   /// streams, create and add remote streams,
   /// etc.)
   ///

   out.add_stream (std::cout);
   warn.add_stream (std::cerr);
   comm.add_stream (std::cerr);

   ///
   /// 6. Declare the groups (if exist)
   ///
   /// The parameter is the identifier of the
   /// group.
   ///
   csu::base::log::group g1("g1");
   csu::base::log::group g2("g2");
   csu::base::log::group g3("g3");

   ///
   /// 7. Declare the levels (if exist)
   ///
   /// The parameter is the identifier of the
   /// level.
   ///
   csu::base::log::level l_gui("l_gui");
   csu::base::log::level l_comm("l_comm");
   csu::base::log::level l_proc("l_proc");


   ///
   /// 8. If we want to add a remote controller
   /// for the log, we have to create a server in
   /// this binary (this part is optional, only
   /// if we need log remote control).
   ///
   /// The server (LogRemoteCtrl object) has to
   /// be created and then started to create the
   /// server thread.
   ///
   /// This step can be performed wherever the
   /// developer wants.
   ///

   LogRemoteCtrl lrc;
   lrc.start();

   ///
   /// Here the code of the application starts
   ///

   while(true)///some test actions
   {
      sleep(3);
      //      system("clear");
      DEBUGc("-");
      DEBUGc("-----");

      out << "salida por el log 'out' normal" << std::endl;
      out << g1 << "salida por el log 'out' en grupo 1" << std::endl;
      out << g2 << "salida por el log 'out' en grupo 2" << std::endl;
      out << g3 << "salida por el log 'out' en grupo 3" << std::endl;
      DEBUGc(".");
      warn << "salida por el log 'warn' normal" << std::endl;
      warn << l_gui << "salida por el log 'warn' - nivel l_gui" << std::endl;
      warn << l_comm << "salida por el log 'warn' - nivel l_comm" << std::endl;
      warn << l_proc << "salida por el log 'warn' - nivel l_proc" << std::endl;
      DEBUGc("-----");
      DEBUGc("-");
   }

}
#endif
