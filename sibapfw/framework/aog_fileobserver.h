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

#ifndef AOG_FILEOBSERVER_H
#define AOG_FILEOBSERVER_H

#include <boost/shared_ptr.hpp>

namespace csu {
namespace aog {
namespace framework {


class FileObserver
{
public:
    typedef boost::shared_ptr<FileObserver> FileObserverPtr;


    virtual void updateFiles() =0;
};

}
}
}
#endif // AOG_FILEOBSERVER_H
