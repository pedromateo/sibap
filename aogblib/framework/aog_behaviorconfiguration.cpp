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

#include "aog_behaviorconfiguration.h"
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
        ("UpdateFileTime", po::value<int>()->implicit_value(3))
        ("LogFormat", po::value<std::string>()->implicit_value("[%tm] %wi Event:%en :: State: %cs Assertion: %ar Function Called: %fc :: Result: %fr"))
        ("LogOutPutFile", po::value< std::vector<std::string> >())
        ("ScriptPath", po::value< std::vector<std::string> >())
        ("StandardError", po::value< std::string >()->implicit_value("off"))
        ("StandardOutput", po::value< std::string >()->implicit_value("on"));
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

        if (vm.count("UpdateFileTime") && (updateFileTime_!=vm["UpdateFileTime"].as< int >()))
        {
           updateFileTime(vm["UpdateFileTime"].as< int >());

        }

        if (vm.count("LogFormat") && (logFormat_!=vm["LogFormat"].as< std::string >()))
        {
            std::string aux=vm["LogFormat"].as< std::string >();
            aux.erase(std::remove( aux.begin(), aux.end(), '\"' ), aux.end());
           logFormat(aux);

        }

        if (vm.count("LogOutPutFile"))
        {
            std::vector<std::string> files=vm["LogOutPutFile"].as< std::vector< std::string > >();
            std::vector<std::string>::iterator i=files.begin();
            clearLogOutputFiles();
            for (;i!=files.end();i++)
            {
                i->erase(std::remove( i->begin(), i->end(), '\"' ), i->end());
                addLogOutputFile(*i);
            }
            logManager->reloadOutPutFiles();
        }

        if (vm.count("ScriptPath"))
        {
            std::vector<std::string> files=vm["ScriptPath"].as< std::vector< std::string > >();
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

        if (vm.count("StandardOutput") && (stadardOutput_!=vm["StandardOutput"].as< std::string >()))
        {
            std::string value=vm["StandardOutput"].as< std::string >();
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

        if (vm.count("StandardError") && (standardError_!=vm["StandardError"].as< std::string >()))
        {
            std::string value=vm["StandardError"].as< std::string >();
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
