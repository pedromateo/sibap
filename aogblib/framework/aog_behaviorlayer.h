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

#ifndef AOG_BehaviorLayer_H
#define AOG_BehaviorLayer_H

#include <framework/aog_forward.h>
#include <framework/aog_fileobserver.h>
#include <framework/aog_eventobserver.h>

namespace csu {
namespace aog {
namespace framework {

    class BehaviorLayer: public csu::aog::framework::EventObserver, public csu::aog::framework::FileObserver
    {
    public:
        BehaviorLayer(BehaviorConfiguration* context);
        void init();

    protected:

       virtual void createElementRegistrar(void) = 0;
       virtual void createBehaviorEngine(void) = 0;
       virtual void createEventAbstractor(void) = 0;
       virtual void createFileManager(void);
       virtual void createLogManager(void);

    private:
        void updateElements(const std::string &windowName){}
        void updateCreateElements(const std::string &windowName);
        void updateDeleteElements(const std::string &windowName);
        void updateFiles(void);


    protected:
        BehaviorConfiguration* context_;
    };

}
}
}

#endif // AOG_BehaviorLayer_H
