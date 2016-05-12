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

#ifndef AOG_FILEOBSERVABLE_H
#define AOG_FILEOBSERVABLE_H

#include <boost/shared_ptr.hpp>
#include <list>
#include <framework/aog_fileobserver.h>

namespace csu {
namespace aog {
namespace framework {

class FileObservable
{
public:
    void registerFileObserver(FileObserver * o)
    {
        fileObList_.push_back(FileObserver::FileObserverPtr(o));
    }

    void notifyAll()
    {
        for (FileObserverList::iterator it = fileObList_.begin();it!=fileObList_.end();it++)
        {
            (*it)->updateFiles();
        }
    }

private:
    typedef std::list<FileObserver::FileObserverPtr> FileObserverList;
    FileObserverList fileObList_;
};

}
}
}
#endif // AOG_FILEOBSERVABLE_H
