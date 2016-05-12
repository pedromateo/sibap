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

#ifndef AOG_EVENTOBSERVABLE_H
#define AOG_EVENTOBSERVABLE_H

#include <boost/shared_ptr.hpp>

#include <boost/ptr_container/ptr_list.hpp>

#include "aog_eventobserver.h"
#include <QEvent>

namespace csu {
namespace aog {
namespace framework {


class EventObservable
{
public:
    void registerObserver(EventObserver * o)
    {
        eventObList_.push_back(o);
    }

    void notifyAll(const std::string & windowName)
    {
        for (EventObserverList::iterator it = eventObList_.begin();it!=eventObList_.end();it++)
        {
            (it)->updateElements(windowName);
        }
    }

    void notifyCreate(const std::string & windowName)
    {
        for (EventObserverList::iterator it = eventObList_.begin();it!=eventObList_.end();it++)
        {
            (it)->updateCreateElements(windowName);
        }
    }

    void notifyDelete(const std::string & windowName)
    {
        for (EventObserverList::iterator it = eventObList_.begin();it!=eventObList_.end();it++)
        {
            (it)->updateDeleteElements(windowName);
        }
    }

private:

    typedef boost::ptr_list<EventObserver> EventObserverList;
    EventObserverList eventObList_;
};

} //framework
} //aog
} //csu


#endif // AOG_EVENTOBSERVABLE_H
