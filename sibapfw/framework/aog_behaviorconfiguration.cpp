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

#include "aog_behaviorconfiguration.h"
#include "aog_definitions.h"
#include <vector>


namespace csu {
namespace aog {
namespace framework {

namespace po = boost::program_options;

BehaviorConfiguration::BehaviorConfiguration(const std::string & configFile) : configFile_(configFile)
{
    updateFileTime_ = 6;

    // Declare a group of options that will be
    // allowed in config file

    configOptions_.add_options()
        (BO_UPDATETIME, po::value<int>()->implicit_value(3))
        (BO_LOGFORMAT, po::value<std::string>()->implicit_value("[%tm] %wi Event:%en :: State: %cs Assertion: %ar Function Called: %fc :: Result: %fr"))
        (BO_LOGOUTPUTFILE, po::value< std::vector<std::string> >())
        (BO_SCRIPTSPATH, po::value< std::vector<std::string> >())
        (BO_STDERR, po::value< std::string >()->implicit_value("off"))
        (BO_STDOUT, po::value< std::string >()->implicit_value("on"));
}


BehaviorConfiguration& BehaviorConfiguration::loadConfigFile(const std::string & configFile)
{
    configFile_ = configFile;
    _reloadConfiguration();
    return *this;
}

const std::string & BehaviorConfiguration::configFile()
{
    return configFile_;
}



const StringList & BehaviorConfiguration::scriptPaths(void)
{
    return paths_;
}

BehaviorConfiguration& BehaviorConfiguration::addScriptPath(const std::string & path)
{
    paths_.push_back(path);
    return *this;
}

BehaviorConfiguration& BehaviorConfiguration::clearScriptPaths()
{
    paths_.clear();
    return *this;
}

int BehaviorConfiguration::updateFileTime()
{
    return updateFileTime_;
}

BehaviorConfiguration& BehaviorConfiguration::updateFileTime(int time)
{
    updateFileTime_=time;
    return *this;
}



///
/// Logmanager config
///

BehaviorConfiguration& BehaviorConfiguration::logFormat(const std::string& f)
{
    logFormat_ = f;
    return *this;
}

const std::string & BehaviorConfiguration::logFormat(void)
{
    return logFormat_;
}

BehaviorConfiguration& BehaviorConfiguration::addLogOutputFile(const std::string& f)
{
    logFiles_.insert(f);
    return *this;
}

BehaviorConfiguration& BehaviorConfiguration::addLogOutputStream(std::ostream& o)
{
    logStreams_.insert(&o);
    return *this;
}

BehaviorConfiguration& BehaviorConfiguration::removeLogOutputStream(std::ostream& o)
{
    logStreams_.erase(&o);
    return *this;
}

BehaviorConfiguration& BehaviorConfiguration::clearLogOutputStreams()
{
    logStreams_.clear();
    return *this;
}

BehaviorConfiguration& BehaviorConfiguration::clearLogOutputFiles()
{
    logFiles_.clear();
    return *this;
}

const std::set<std::string> & BehaviorConfiguration::logFiles(void)
{
    return logFiles_;
}

const std::set<std::ostream*> & BehaviorConfiguration::logStreams(void)
{
    return logStreams_;
}


///Boost Program_Options since 1.42 does not remove quotations, thats why it is needed to do
bool BehaviorConfiguration::_reloadConfiguration()
{
    std::ifstream cfile(configFile_.c_str());

    try
    {

        po::variables_map vm;
        po::store(po::parse_config_file(cfile, configOptions_,true), vm);
        po::notify(vm);

        po::variables_map::iterator i;

        if (vm.count(BO_UPDATETIME) && (updateFileTime_!=vm[BO_UPDATETIME].as< int >()))
        {
           updateFileTime(vm[BO_UPDATETIME].as< int >());

        }

        if (vm.count(BO_LOGFORMAT) && (logFormat_!=vm[BO_LOGFORMAT].as< std::string >()))
        {
            std::string aux=vm[BO_LOGFORMAT].as< std::string >();
            aux.erase(std::remove( aux.begin(), aux.end(), '\"' ), aux.end());
           logFormat(aux);

        }

        if (vm.count(BO_LOGOUTPUTFILE))
        {
            std::vector<std::string> files=vm[BO_LOGOUTPUTFILE].as< std::vector< std::string > >();
            std::vector<std::string>::iterator i=files.begin();
            clearLogOutputFiles();
            for (;i!=files.end();i++)
            {
                i->erase(std::remove( i->begin(), i->end(), '\"' ), i->end());
                addLogOutputFile(*i);
            }
            logManager->reloadOutPutFiles();
        }

        if (vm.count(BO_SCRIPTSPATH))
        {
            std::vector<std::string> files=vm[BO_SCRIPTSPATH].as< std::vector< std::string > >();
            std::vector<std::string>::iterator i=files.begin();

            //Check if contents are different
            //FIXME Change
            bool different=true;

            if (files.size()==paths_.size())
            {

                for (i=files.begin();i!=files.end();i++)
                {
                    i->erase(std::remove( i->begin(), i->end(), '\"' ), i->end());
                    boost::filesystem::path p1(*i);
                    different=true;
                    for (StringList::iterator itlist=paths_.begin();itlist!=paths_.end();itlist++)
                    {
                        boost::filesystem::path p2(*itlist);

                        //We compare parent paths to avoid wrong comparission between ./dir1 and ./dir1/
                        if (p1.parent_path()!=p2.parent_path())
                        {
                            different=false;
                            break;
                        }

                    }
                    if (different)
                        break;
                }
            }

            if (different)
            {
                //This control if it is the first time the config is loaded
                //beacuse there is no need to notify file change to the file observers
                bool firsttime=true;

                if (scriptPaths().size()!=0)
                    firsttime=false;


                //Removing context script paths
                clearScriptPaths();

                for (i=files.begin();i!=files.end();i++)
                {
                    i->erase(std::remove( i->begin(), i->end(), '\"' ), i->end());
                    addScriptPath(*i);
                }

                 //Cleaning stored script paths
                fileManager->clearFiles();

                //If it is not the first time then notifyall
                if (fileManager->initiated())
                {
                    //Calling to update files
                    fileManager->updateFiles();
                    notifyAll();
                }
            }
        }

        if (vm.count(BO_STDOUT) && (stadardOutput_!=vm[BO_STDOUT].as< std::string >()))
        {
            std::string value=vm[BO_STDOUT].as< std::string >();
            value.erase(std::remove( value.begin(), value.end(), '\"' ), value.end());

            if (value=="on")
                addLogOutputStream(std::cout);
            else
                removeLogOutputStream(std::cout);

            //Save the new value
            stadardOutput_=value;

            //Making the logmanager to reload streams
            logManager->reloadOutPutStreams();
        }

        if (vm.count(BO_STDERR) && (standardError_!=vm[BO_STDERR].as< std::string >()))
        {
            std::string value=vm[BO_STDERR].as< std::string >();
            value.erase(std::remove( value.begin(), value.end(), '\"' ), value.end());

            if (value=="on")
                addLogOutputStream(std::cerr);
            else
                removeLogOutputStream(std::cerr);

            //Save the new value
            standardError_=value;

            //Making the logmanager to reload streams
            logManager->reloadOutPutStreams();
        }


    }catch(const po::invalid_syntax &error)
    {
        //std::cout<<"Error: "<< /* FIXME error.kind()*/ 0 <<" in option '"<<error.tokens()<<"'" << std::endl;
        return false;
    }catch(const po::error &error)
    {
        std::cout<<"Configuration Syntax Error" << std::endl;
        return false;
    }
    return true;
}

}
}
}
