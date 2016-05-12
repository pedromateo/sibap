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

#ifndef AOG_LOGMANAGER_H
#define AOG_LOGMANAGER_H

#include <framework/aog_forward.h>
#include <config/log_config_internal.h>
#include <boost/ptr_container/ptr_set.hpp>


namespace csu {
namespace aog {
namespace framework {

#include <config/log/log.hpp>

class LogManager
{
public:
    ///
    /// Constructor
    /// @param context is the aspect context
    ///
    LogManager(BehaviorConfiguration* context);
    virtual ~LogManager();

    ///
    /// installation methods
    ///
    void install();
    void uninstall();
    void reloadOutPutFiles();
    void reloadOutPutStreams();

    ///
    /// adds a new entry in the log
    ///
    //FIXME CHANGE
    std::string doLog(const ExecutionResultPtr er);
    void doLog(const std::string & statement);
    void doLog(const std::string & logFile,const std::string & statement);

protected:
    ///
    /// prints the result in all the log outputs
    ///
    void dispatchResult(const std::string & r);

private:
    BehaviorConfiguration* context_;
    /// log object
    csu::base::log::logger<true> logFiles_;
    csu::base::log::logger<true> logStreams_;
    /// log files streams
    typedef boost::ptr_set<std::ofstream> StreamSet;
    StreamSet files_;
    /// open log files
    typedef std::map<std::string,std::ofstream *> OpenFileMap;
    OpenFileMap openLogFiles_;



};

}
}
}
#endif // AOG_LOGMANAGER_H


