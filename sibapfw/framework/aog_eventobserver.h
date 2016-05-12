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

#ifndef AOG_EVENTOBSERVER_H
#define AOG_EVENTOBSERVER_H

#include <boost/shared_ptr.hpp>

namespace csu {
namespace aog {
namespace framework {

class EventObserver
{
public:
    typedef boost::shared_ptr<EventObserver> EventObserverPtr;

    virtual void updateElements(const std::string & windowName) = 0;
    virtual void updateCreateElements(const std::string & windowName) = 0;
    virtual void updateDeleteElements(const std::string & windowName) = 0;
};

} //framework
} //aog
} //csu

#endif // AOG_EVENTOBSERVER_H
