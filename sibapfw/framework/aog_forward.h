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

#ifndef AOG_FORWARD_H
#define AOG_FORWARD_H

#include <list>
#include <set>
#include <string>

#include <boost/shared_ptr.hpp>

namespace csu {
namespace aog {
namespace framework {

    typedef std::list<std::string> StringList;

    class BehaviorConfiguration;
    //typedef boost::shared_ptr<BehaviorConfiguration> BehaviorConfigurationPtr;

    class GenericElementRegistrar;
    typedef boost::shared_ptr<GenericElementRegistrar> GenericElementRegistrarPtr;

    class GenericBehaviorEngine;
    typedef boost::shared_ptr<GenericBehaviorEngine> GenericBehaviorEnginePtr;

    class GenericFileManager;
    typedef boost::shared_ptr<GenericFileManager> GenericFileManagerPtr;

    class GenericEventAbstractor;
    typedef boost::shared_ptr<GenericEventAbstractor> GenericEventAbstractorPtr;

    class LogManager;
    typedef boost::shared_ptr<LogManager> LogManagerPtr;

    class ExecutionResult;
    typedef boost::shared_ptr<ExecutionResult> ExecutionResultPtr;

    typedef std::list<std::string> FunctionList;
    typedef std::set<std::string> FunctionSet;

}
}
}
#endif // AOG_FORWARD_H
