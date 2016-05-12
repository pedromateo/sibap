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

#include <framework/aog_logmanager.h>
#include <framework/aog_behaviorconfiguration.h>
#include <framework/aog_executionresult.h>

namespace csu {
namespace aog {
namespace framework {

LogManager::LogManager(BehaviorConfiguration *context) : context_(context)
{
    csu::aog::config::log::LogConfigInternal::initialize();

}

LogManager::~LogManager(){}

///
/// installation methods
///
void LogManager::reloadOutPutFiles()
{
    //flush and close all streams
    StreamSet::iterator itStream;

    for (itStream = files_.begin();
    itStream != files_.end();
    itStream++)
    {
        (*itStream).flush();
        (*itStream).close();
    }

    logFiles_.state().streams.clear();
    files_.release();

    std::set<std::string>::iterator it;
    std::set<std::string> files= context_->logFiles();

    for (it = files.begin();
    it != files.end();
    it++)
    {
        //create a file stream
        std::ofstream* o =new std::ofstream;

        o->open((*it).c_str(), std::ios::app);
        files_.insert(o);

        //add the stream to the log
        logFiles_.add_stream(*o);
    }
}

void LogManager::reloadOutPutStreams()
{
    logStreams_.state().streams.clear();

    ///add streams if needed
    std::set<std::ostream*>::iterator ito;
    std::set<std::ostream*> streams= context_->logStreams();
    for (ito = streams.begin();
    ito != streams.end();
    ito++)
    {
        //add the stream to the log
        std::ostream* os = *ito;
        logStreams_.add_stream(*os);
    }

}

void LogManager::install()
{
    ///add files if needed
    std::set<std::string>::iterator it;
    std::set<std::string> files= context_->logFiles();
    for (it = files.begin();
    it != files.end();
    it++)
    {
        //create a file stream
        std::ofstream* o =new std::ofstream;

        o->open((*it).c_str(), std::ios::app);
        files_.insert(o);

        //add the stream to the log
        logFiles_.add_stream(*o);
    }

    ///add streams if needed
    std::set<std::ostream*>::iterator ito;
    std::set<std::ostream*> streams= context_->logStreams();
    for (ito = streams.begin();
    ito != streams.end();
    ito++)
    {
        //add the stream to the log
        std::ostream* os = *ito;
        logStreams_.add_stream(*os);
    }

    logFiles_ << csu::base::log::on;
    logStreams_ << csu::base::log::on;
}

void LogManager::uninstall()
{
    //flush and close all streams
    StreamSet::iterator it;
    for (it = files_.begin();
    it != files_.end();
    it++)
    {
        (*it).flush();
        (*it).close();
        logFiles_.remove_stream(*it);
    }

    files_.release();

    logFiles_.state().streams.clear();
    logStreams_.state().streams.clear();

    logFiles_ << csu::base::log::off;
    logStreams_ << csu::base::log::off;

    //Flushing and closing on demand streams opened
    OpenFileMap::iterator itmap=openLogFiles_.begin();

    for (;itmap!=openLogFiles_.end();itmap++)
    {
        itmap->second->flush();
        itmap->second->close();
    }
}

///
/// doLog method
///
std::string LogManager::doLog(const ExecutionResultPtr er)
{
    std::string base = context_->logFormat();
    std::string result = "";
    std::string aux = "";

    ///
    /// Log Format:
    /// - %tm -> timestamp
    /// - %en -> event name
    /// - %wi -> widget interacted
    /// - %cs -> current state
    /// - %ar -> assert Result
    /// - %fc -> function called
    /// - %fr -> function result


    ///do the base string char by char looking
    ///for format symbols
    std::string::iterator itbase = base.begin();
    while (itbase != base.end())
    {
        //on format sequence...
        if (*itbase == '%')
        {
            itbase++; aux = *itbase;
            itbase++; aux += *itbase;
            //check it
            if (aux == "tm")
            {
                boost::posix_time::ptime t(boost::posix_time::second_clock::local_time());
                result += boost::posix_time::to_simple_string(t);
            }
            else if (aux == "en")
            {
                result += er->actionEvent();
            }
            else if (aux == "wi")
            {
                result += er->actionWidget();
            }
            else if (aux == "cs")
            {
                result += er->currentState();
            }
            else if (aux == "ar")
            {
                if (er->assertResult())
                    result +="Satisfied";
                else
                    result +="Not Satisfied";
            }
            else if (aux == "fc")
            {
                result +=er->functionCalled();
            }
            else if (aux == "fr")
            {
                if (er->functionResult())
                    result +="True";
                else
                    result +="False";
            }
            else
            {
                itbase--;itbase--;
                result += *itbase;
            }
        }
        //regular character
        else
        {
            result += *itbase;
        }

        //go with the next
        itbase++;
    }

    ///dispatch the result
    dispatchResult(result);

    //return the complete log entry
    return result;
}

void LogManager::doLog(const std::string & statement)
{
    std::string result;

    //Formatting the statement to add the date
    boost::posix_time::ptime t(boost::posix_time::second_clock::local_time());
    result +="[";
    result += boost::posix_time::to_simple_string(t);
    result +="] Internal Script Log: ";
    result +=statement;

    dispatchResult(result);
}

void LogManager::doLog(const std::string & logFile,const std::string & statement)
{

    std::string result;

    //Formatting the statement to add the date
    boost::posix_time::ptime t(boost::posix_time::second_clock::local_time());
    result +="[";
    result += boost::posix_time::to_simple_string(t);
    result +="] Internal Script Log: ";
    result +=statement;


    //create and open the file stream if it doesnt exists
    if (!openLogFiles_[logFile]){
        openLogFiles_[logFile]= new std::ofstream;
        openLogFiles_[logFile]->open(logFile.c_str(), std::ios::app);
    }

   if (openLogFiles_[logFile])
        *(openLogFiles_[logFile])<<result<<std::endl;


}

///
/// prints the result in all the log outputs
///
void LogManager::dispatchResult(const std::string& r)
{
    logFiles_ << r <<std::endl;
    logStreams_ << r <<std::endl;
}

}
}
}
