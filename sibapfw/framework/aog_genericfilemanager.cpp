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

#include "aog_genericfilemanager.h"

#include <framework/aog_behaviorconfiguration.h>
#include <config/log_config_internal.h>

namespace csu {
namespace aog {
namespace framework {

using namespace csu::aog::config::log;

GenericFileManager::WatchDog::WatchDog(GenericFileManager *manager)
:manager_(manager)
{
    monitoring_ = false;
}

void GenericFileManager::WatchDog::start()
{
    if (manager_->context_->updateFileTime()==0)
        return;

    monitoring_=true;
    rthread_=new boost::thread(boost::bind(&WatchDog::monitor,this));
}

void GenericFileManager::WatchDog::stop()
{
    //_log::debug << "(RuleManager::WatchDog::stop)" << std::endl;
    if (!monitoring_)
        return;
    monitoring_ = false;
    rthread_->detach();
    rthread_->interrupt();
    delete rthread_;
    rthread_ = NULL;
}

void GenericFileManager::WatchDog::monitor()
{
    while (monitoring_)
    {
        //Always get the time stored in the context_
        //because any modification will be in that place
        boost::this_thread::sleep(boost::posix_time::seconds(manager_->context_->updateFileTime()));
        manager_->updateConfig();
        manager_->updateFiles();
    }
}

GenericFileManager::GenericFileManager(BehaviorConfiguration *context):context_(context)
{
    _log::debug << "GenericFileManager::constructor" << std::endl;
    watchdog_= new WatchDog(this);
    unlock();
    initiated_=false;

}

GenericFileManager::~GenericFileManager()
{
    watchdog_->stop();
}

void GenericFileManager::lock()
{
    locked_=true;
}

void GenericFileManager::unlock()
{
    locked_=false;
}

bool GenericFileManager::locked()
{
    return locked_;
}

bool GenericFileManager::initiated()
{
    return initiated_;
}

const StringList * GenericFileManager::files()
{
    if (!locked())
        return &fileList_;
    else
        return NULL;
}

///
/// Get the last write time
/// @param path_ the path to the file
/// @return the last write time of the file
///
std::string GenericFileManager::getFileLastDate(boost::filesystem::path path)
{
    time_t lastModified = boost::filesystem::last_write_time(path);
    return (ctime(&lastModified));  // convert to c++ a std::string;
}

bool GenericFileManager::hasChanged(const std::string & file,const std::string & storedLastWrite)
{
    bool changed = false;

    std::string lastWrite = getFileLastDate(file); // get the last write time

    if (lastWrite.compare(storedLastWrite) != 0)
    {   // if dates are different
        changed = true;                   // the file has changed
        _log::script << "(GenericFileManager::hasChanged) File " << file << " has changed" << std::endl;
    }

    return changed;
}

///
/// Service Methods
///

void GenericFileManager::clearFiles()
{
    //Cleaning stored script paths
    fileList_.clear();
    fileTimeMap_.clear();
}

bool GenericFileManager::updateConfig()
{
    using namespace boost::filesystem;


    if (!initiated_)
    {
        configFile_ = context_->configFile();
        //If it is does not exist
        if (!exists(path(configFile_)))
        {
            _log::script <<"(FileManager::updateConfig) Error: Config File '"<<configFile_<<"'does not exists." << std::endl;
            configFile_ = "";
            return false;
        }

        context_->loadConfigFile(configFile_);
        lastWriteConfigFile_ = getFileLastDate(path(configFile_));
        return true;
    }else{
        //No config file present
        if (configFile_=="")
            return false;

        //This case will only ocurr when the config file has been deleted or moved
        if (!exists(path(configFile_)))
            return false;

        //If date changed then reload config file
        if (hasChanged(configFile_,lastWriteConfigFile_)){
             context_->loadConfigFile(configFile_);
             lastWriteConfigFile_ = getFileLastDate(path(configFile_));
             return true;
        }
    }
    return false;
}



///
/// The first call will extract the path of all script files
/// then, check if any file has been modified and reload.
/// @return true if is there any change in the rules
///
bool GenericFileManager::updateFiles()
{
    using namespace boost::filesystem;

    //Lock the access
    if (locked())
        return false;

    //Set the lock
    lock();

    bool updated = false;

    //Needed to be outside the global if
    if (!initiated_)
        updateConfig();

    loadFiles(context_->scriptPaths());

    ///
    /// first time
    if(!initiated_)
    {
        initiated_ = true;

        // start the files watchdog
        watchdog_->start();
    }
    ///
    /// next times: explore modified files
    else if (!fileTimeMap_.empty())
    {
        // not first time: check if any file have been modified and reload it
        StringList::iterator it;
        StringList toReload;
        StringList::iterator it_toReload;

        // check for changes in files
        for(it = fileList_.begin(); it != fileList_.end(); )
        {
            //Checking if the file still exists
            if (!exists(*it))
            {
                //Removing non-existent files
                fileTimeMap_[*it].clear();
                fileTimeMap_.erase(*it);
                //Erase operations in stl containers returns the next iterator
                it=fileList_.erase(it);
                continue;
            }
            // if changed
            if (hasChanged(*it,fileTimeMap_[*it]))
            {
                // mark the file to reload
                toReload.push_back(*it);
                // and notify
                updated = true;
                _log::script <<"(FileManager::updateFiles) file " << *it << " mark to reload" << std::endl;
            }
            it++;
        }

        // Reload files with changes
        for(it_toReload = toReload.begin();it_toReload != toReload.end();it_toReload++)
        {
            path p=path(*it_toReload);
            // Finally reload the files
            std::string lastModified = getFileLastDate(p);
            fileTimeMap_[p.string()] = lastModified;
        }

    }

    //Release lock
    unlock();


    if (!updated)
       ;//_log::script <<"(FileManager::updateFile) No files updated since last checking"<< std::endl;
    else
        notifyAll();

    return updated;
}


bool GenericFileManager::loadDirectory(boost::filesystem::path dir_path)
{
    using namespace boost::filesystem;

    directory_iterator end_iter;

    if ( !exists( dir_path ))
        return false;

    // Iterate over it
    for (directory_iterator dir_itr(dir_path);dir_itr != end_iter;++dir_itr)
    {

        if(is_directory(dir_itr->status()))
        {
            //dont load files in hidden dirs
            if (dir_itr->path().filename().string()[0] != '.')
                loadDirectory(dir_itr->path());
        }else{
            loadSingleFile(dir_itr->path());
        }
    }
    return true;
}

///
/// Check if each path is a file or a directory,
/// iterate over directories and load all the files found
/// @param paths names of the paths for search
///

void GenericFileManager::loadFiles(StringList paths)
{
    using namespace boost::filesystem;
    _log::script  << "(GenericFileManager::loadFiles) (Re)Loading " << paths.size() << " files." << std::endl;
    // For each path ...
    StringList::iterator it;

    for(it=paths.begin(); it != paths.end(); it++)
    {
        path user_path((*it));

        // if the path is a directory
        if (is_directory(user_path))
        {
            loadDirectory(user_path);

        } // end if
        else
        {
            // the path is a single file
            //std::cout<<"Watchout: path "<<user_path.filename()<<" is a SINGLE FILE" << std::endl;
        }
    } // end for
}


///
/// Load a single file, separate the file into rules using a prefix.
/// Add the file to the map of <files,last write date>
/// @param path path of the file to load
///
bool GenericFileManager::loadSingleFile(boost::filesystem::path path)
{
    using namespace boost::filesystem;
    //_log::script  << "(GenericFileManager::loadSingleFile) "<<path.file_string()<<std::endl;

    //if it doesnt exist return
    if (!exists(path))
        return false;

    //if it is not a regular file
    if (!is_regular_file(path))
        return false;

    //FIXME TODO
    //FIXME WATCH THIS!!!!!!!!!!!
    //Filter by extension
    // if it is not a specific file of the language used, return
    //    if (extension(path)!=suffix_)
    //        return false;

    //    if (((extension(path)!=suffix_) &&
    //         (extension(path)!=".txt")))
    //        return false;

    if (fileTimeMap_[path.string()]=="")
    {
        fileList_.push_back(path.string());
        fileTimeMap_[path.string()] = getFileLastDate(path);
    }

    return true;
}



}
}
}
