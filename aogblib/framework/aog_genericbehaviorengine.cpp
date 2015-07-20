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

#include "aog_genericbehaviorengine.h"
#include <framework/aog_executionresult.h>

#include <boost/bind.hpp>



namespace csu {
namespace aog {
namespace framework {

using namespace csu::aog::config::log;

GenericBehaviorEngine::GenericBehaviorEngine(BehaviorConfiguration *context):context_(context)
{
    _log::debug << "GenericBehaviorEngine::constructor" << std::endl;

    separator="__";

    initER="init"+separator+"[a-zA-Z0-9_-]+";
    stateER="(state_[a-zA-Z0-9_-]+)"+separator+"([a-zA-Z0-9_-]+)";
    stateactionER="(state_[a-zA-Z0-9_-]+)"+separator+"([a-zA-Z0-9_-]+)"+separator+"[a-zA-Z0-9_-]+";
    logwidgetER="log"+separator+"[a-zA-Z0-9_-]+";
    logactionER="([a-zA-Z0-9_-]+)"+separator+"log";
    defaultER="([a-zA-Z0-9_-]+)"+separator+"+([a-zA-Z0-9_-]+)";

    //FIXME TODO This RE is temporal waiting to make a regular expression like state_s__action__widget
    definedFuncER="[a-zA-Z0-9_-]+"+separator+"[a-zA-Z0-9_-]+";

    stmachine_=new StateMachine(context_);

}

void GenericBehaviorEngine::loadScripts(const StringList * scripts)
{
    //First load the scripts into the scripting engine, this function returns
    //the functions loaded
    FunctionSet * funcs=const_cast<FunctionSet *>(loadScriptsinEngine(scripts));

    //Process the loaded functions
    processGlobalEnvironmentTable(funcs);

    /// **********************************************
    /// Registering Internal Functions into the engine
    /// **********************************************

    //Registering internal log functions
    void (LogManager::*globalLog)(const std::string &) = &LogManager::doLog;
    void (LogManager::*specificLog)(const std::string &,const std::string &) = &LogManager::doLog;

    registerLogGlobalFunc(INTERNALLOG,boost::function<void (std::string)> (boost::bind(globalLog,context_->logManager,_1)));
    registerLogSpecificFunc(INTERNALFILELOG,boost::function<void (std::string,std::string)> (boost::bind(specificLog,context_->logManager,_1,_2)));

}


void GenericBehaviorEngine::initializeEngine()
{
    initialize();
    stmachine_->init();
}

bool GenericBehaviorEngine::callAssertFunction(const std::string &widget)
{
    return callFunction("Assert"+separator+widget);
}

bool GenericBehaviorEngine::executeFunction(const std::string & function)
{
    ExecutionResultPtr er(new ExecutionResult());
    er->functionCalled(function);
    er->currentState(const_cast<State *>(stmachine_->currentState())->id());
    context_->logManager->doLog(er);
    er.reset();
    return callFunction(function);
}

bool GenericBehaviorEngine::executeFunction(const FunctionSet & funcList)
{
    return callFunction(funcList);
}

bool GenericBehaviorEngine::executeAction(const std::string & action,const std::string & widget)
{
    _log::behav << "(GenericBehaviorEngine::executeAction) Action "
                << action << " from widget " << widget << " arrived." << std::endl;

    FunctionSet * fset = &widgetFuncMap_[widget];

    if (fset->size()!=0)
    {
        ExecutionResultPtr er(new ExecutionResult());
        std::string defaultaction=action+separator+widget;
        std::string stateaction;
        std::string realfunc=defaultaction;
        State * current = const_cast<State *>(stmachine_->currentState());

        if (current!=stmachine_->initState())
        {
            stateaction=current->id()+separator+defaultaction;
            //Check if the current state function exists, else call the default function
            if (functionExists(stateaction))
                realfunc=stateaction;
        }


        //To avoid unnecessary calls, a previous check is done
        //This checking is done by indexing the global table of the engine itself
        //beacuse searches have a better performance
        if (!functionExists(realfunc))
           return false;

        //Calling assertion function if exists
        bool assertion = this->callAssertFunction(widget);

        bool result;
        if (assertion){
            result = this->callFunction(realfunc);
        }

        er->assertResult(assertion);
        er->functionCalled(realfunc);
        er->actionWidget(widget);
        er->actionEvent(action);
        er->currentState(current->id());
        er->functionResult(result);
        context_->logManager->doLog(er);
        er.reset();

        //Once the function is called, additional log functions for auditing are called
        //First:  functions like action__log
        std::string logFunc=action+separator+"log";

        if (functionExists(logFunc))
            this->callFunction(logFunc);

        //Second: functions like log__widget
        logFunc="log"+separator+widget;
        if (functionExists(logFunc))
            this->callFunction(logFunc);
    }

}

const FunctionSet & GenericBehaviorEngine::functions()
{
    return globalFuncList_;
}


void GenericBehaviorEngine::processGlobalEnvironmentTable(const FunctionSet * funcs)
{

    boost::cmatch what;
    FunctionSet initFuncList_;
    FunctionSet funcList_;

    widgetFuncMap_.clear();
    globalFuncList_.clear();


    //At this point the state machine could be in two different stages:
    // -- Initial State. No problem, loading all functions and states
    // -- Other State. This situation will be present when reloading script files. In this stage,
    // we have to take care of inconsistency. If we reload and ther is no function associated with the current
    // state that is a inconsistency. We will show a warning prompt but the program will not stop. Why? beacuse we can
    // add these functions easily in the script files
    if (stmachine_->initState()!=stmachine_->currentState())
    {
        //There is no need to delete registered transition functions in the engine
        //because every time this function is called, the engine is clean
        stmachine_->clear();
    }


    for (FunctionSet::const_iterator it=funcs->begin();it!=funcs->end();it++)
    {

        //Inserting the new function into the global function list
        globalFuncList_.insert(*it);

        if (boost::regex_match((*it).c_str(),what,initER))
        {
            initFuncList_.insert(*it);
            widgetFuncMap_[what[1]].insert(*it);
        }else if (boost::regex_match((*it).c_str(),what,logwidgetER))
        {
            continue;
        }else if (boost::regex_match((*it).c_str(),what,logactionER)){
            continue;
        }else if (boost::regex_match((*it).c_str(),what,stateER))
        {
            boost::cmatch stateaction;
            bool initFunc=true;

            /// In what[1] the state
            /// In what[2] the widget

            std::string state=what[1];
            std::string widget=what[2];

            widgetFuncMap_[widget].insert(*it);

             //Check if it is a state action statement (state_a__clic__widget)
            if (boost::regex_match((*it).c_str(),stateaction,stateactionER))
            {
                state=stateaction[1];
                initFunc=false;
            }

            State * stOb;

            if (!(stOb=const_cast<State *>(stmachine_->state(state))))
            {
                _log::behav<<"(GenericBehaviorEngine::processGlobalEnvironmentTable) New state detected, Registering..." << std::endl;
                stOb=const_cast<State *>(stmachine_->newState(state));
                assert(stOb);
            }

            //Second: store this function inside the functionset of functions of this state
            (initFunc)?stOb->addInitFunc(*it):stOb->addActionFunc(*it);

        }else if (boost::regex_match((*it).c_str(),what,defaultER))
        {
            //In what[1] the action
            if (!context_->eventAbstractor->isValidEvent(what[1]))
                continue;

            funcList_.insert(*it);
            widgetFuncMap_[what[2]].insert(*it);

        }else{
            continue;
        }
    }


    //Register the function that make the transition to the different states
    //The function name will be something like state_STATENAME()
    StateMachine::StateList * states=const_cast<StateMachine::StateList *>(stmachine_->states());
    for (StateMachine::StateList::iterator itstate=states->begin();itstate!=states->end();itstate++)
    {
        registerFunction((*itstate)->id(),boost::bind(&StateMachine::makeTransition,stmachine_,(*itstate)));
    }


    //Adding init functions to the init state
    (const_cast<State *>(stmachine_->initState()))->addInitFunc(initFuncList_);

    //Adding action functions to the init state
    (const_cast<State *>(stmachine_->initState()))->addActionFunc(funcList_);

    //Checking if the state machine is in a consistent state
    if (!stmachine_->checkConsistency())
        context_->logManager->doLog("*** WARNING *** State Machine is it in a INCONSISTENT state. Current state is '"
                                    +const_cast<State *>(stmachine_->currentState())->id()+"' and it doesnt exist in the script files.");


}



}
}
}
