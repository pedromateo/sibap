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

#include "aog_behaviorlayer.h"

#include <framework/aog_behaviorconfiguration.h>
#include <framework/aog_genericbehaviorengine.h>
#include <framework/aog_genericfilemanager.h>
#include <framework/aog_genericeventabstractor.h>
#include <framework/aog_genericelementregistrar.h>


namespace csu {
namespace aog {
namespace framework {

using namespace csu::aog::config::log;

BehaviorLayer::BehaviorLayer(BehaviorConfiguration* context) : context_(context)
{
    std::cout << "----------------------------------------------------------------------" << std::endl;
    std::cout << "(BehaviorLayer) Configuration:" << std::endl;
    std::cout << "------------------------------" << std::endl;
    std::list<std::string>::const_iterator it = context_->scriptPaths().begin();
    while (it != context_->scriptPaths().end()){
        std::cout << "(BehaviorLayer) script folder = " << *it << std::endl;
        it++;
    }
    std::cout << "(BehaviorLayer) updateFileTime = " << context->updateFileTime() << std::endl;
    std::cout << "(BehaviorLayer) logFormat = " << context->logFormat() << std::endl;
    std::set<std::string>::const_iterator it2 = context_->logFiles().begin();
    while (it2 != context_->logFiles().end()){
        std::cout << "(BehaviorLayer) log file = " << *it2 << std::endl;
        it2++;
    }
    std::cout << "----------------------------------------------------------------------" << std::endl << std::endl;
}

void BehaviorLayer::init()
{
    createFileManager();
    assert(context_->fileManager.get());

    createElementRegistrar();
    assert(context_->elementRegistrar.get());

    createBehaviorEngine();
    assert(context_->aspectEngine.get());

    createEventAbstractor();
    assert(context_->eventAbstractor.get());

    createLogManager();
    assert(context_->logManager.get());

    std::cout <<"(BehaviorLayer::init) Modules created." << std::endl;

    //Installing the Log Manager
    context_->logManager->install();

    std::cout <<"(BehaviorLayer::init) LogManager installed." << std::endl;

    //Updating the script files
    context_->fileManager->updateFiles();

    std::cout <<"(BehaviorLayer::init) Script files loaded." << std::endl;

    //Initalize engine and load the script files into it
    context_->aspectEngine->initializeEngine();

    std::cout <<"(BehaviorLayer::init) Engine initialized." << std::endl;

    //Install the event abstractor
    context_->eventAbstractor->install();

    std::cout <<"(BehaviorLayer::init) EventAbstractor installed." << std::endl;

    context_->registerFileObserver(this);
    context_->fileManager->registerFileObserver(this);
    context_->eventAbstractor->registerObserver(this);

    std::cout  <<"(BehaviorLayer::init) Initialized." << std::endl;
    std::cout << "----------------------------------------------------------------------" << std::endl << std::endl;
}

void BehaviorLayer::createFileManager(void)
{
    context_->fileManager.reset(new GenericFileManager(context_));
}

void BehaviorLayer::createLogManager(void)
{
    context_->logManager.reset(new LogManager(context_));
}

void BehaviorLayer::updateCreateElements(const std::string &windowName)
{
    context_->elementRegistrar->windowCreate(windowName);

}

void BehaviorLayer::updateDeleteElements(const std::string &windowName)
{
    context_->elementRegistrar->windowDelete(windowName);
}

void BehaviorLayer::updateFiles(void)
{
    //This function will reset the scripting engine and reload the script files
    context_->aspectEngine->resetEngine();
}


}
}
}
