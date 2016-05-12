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

#include "aog_genericeventabstractor.h"

namespace csu {
namespace aog {
namespace framework {


GenericEventAbstractor::GenericEventAbstractor(BehaviorConfiguration *context) : context_(context)
{}

bool GenericEventAbstractor::isValidEvent(const std::string & event)
{
    EventMap::iterator it=eventMap_.begin();

    for (;it!=eventMap_.end();it++)
    {
        if (it->second==event)
            return true;
    }
    return false;
}

}
}
}
