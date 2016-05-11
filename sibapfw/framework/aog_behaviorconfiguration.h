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

#ifndef AOG_BehaviorConfiguration_H
#define AOG_BehaviorConfiguration_H

#include <framework/aog_fileobservable.h>

#include <framework/aog_logmanager.h>
#include <framework/aog_genericbehaviorengine.h>
#include <framework/aog_genericelementregistrar.h>
#include <framework/aog_genericeventabstractor.h>
#include <framework/aog_genericfilemanager.h>

#include <boost/program_options.hpp>


namespace csu {
namespace aog {
namespace framework {



class BehaviorConfiguration : public FileObservable
{
public:
    BehaviorConfiguration(const std::string & configFile="");

    BehaviorConfiguration& loadConfigFile(const std::string & configFile);
    const std::string & configFile();


    const StringList & scriptPaths(void);
    BehaviorConfiguration& addScriptPath(const std::string & path);
    BehaviorConfiguration& clearScriptPaths();

    int updateFileTime();
    BehaviorConfiguration& updateFileTime(int);

    ///
    /// Logmanager config
    ///

    /// Log Format:
    /// - %tm -> timestamp
    /// - %en -> event name
    /// - %wi -> widget interacted
    /// - %cs -> current state
    /// - %ar -> assert Result
    /// - %fc -> function called
    /// - %fr -> function result


    BehaviorConfiguration& logFormat(const std::string&);
    const std::string & logFormat(void);
    BehaviorConfiguration& addLogOutputFile(const std::string&);
    BehaviorConfiguration& addLogOutputStream(std::ostream&);
    BehaviorConfiguration& removeLogOutputStream(std::ostream&);
    BehaviorConfiguration& clearLogOutputStreams();
    BehaviorConfiguration& clearLogOutputFiles();
    const std::set<std::string> & logFiles(void);
    const std::set<std::ostream*> & logStreams(void);

protected:
    bool _reloadConfiguration();

protected:
    std::string logFormat_;
    std::set<std::string> logFiles_;
    std::set<std::ostream*> logStreams_;


public:
    GenericElementRegistrarPtr elementRegistrar;
    GenericEventAbstractorPtr eventAbstractor;
    GenericBehaviorEnginePtr aspectEngine;
    GenericFileManagerPtr fileManager;
    LogManagerPtr logManager;


protected:
    ///
    /// ElementRegistrar config
    ///

    /// script files path
    std::string configFile_;
    StringList paths_;
    int updateFileTime_;
    std::string stadardOutput_,standardError_;

    boost::program_options::options_description configOptions_;
    boost::program_options::variables_map lastConfigOptions_;

};


}
}
}

#endif // AOG_BehaviorConfiguration_H
