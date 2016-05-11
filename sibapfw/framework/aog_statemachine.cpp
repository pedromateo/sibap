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

#include "aog_statemachine.h"
#include <framework/aog_behaviorconfiguration.h>

using namespace csu::aog::config::log;

namespace csu {
namespace aog {
namespace framework {

State::State(const std::string & id):id_(id)
{}

const std::string & State::id()
{
    return id_;
}

void State::addInitFunc(const std::string & func)
{
    initFuncList_.insert(func);
}

void State::addInitFunc(const FunctionSet & funcList)
{
    initFuncList_.insert(funcList.begin(),funcList.end());
}

void State::addActionFunc(const std::string & func)
{
    funcList_.insert(func);
}

void State::addActionFunc(const FunctionSet & funcList)
{
    funcList_.insert(funcList.begin(),funcList.end());
}

void State::clear()
{
    funcList_.clear();
    initFuncList_.clear();
}

const FunctionSet & State::initFunctions()
{
    return initFuncList_;
}

const FunctionSet & State::actionFunctions()
{
    return funcList_;
}

//##############################################################################################


StateMachine::StateMachine(BehaviorConfiguration *context):context_(context)
{
    currentState_ = NULL;
    initState_ = new State("init");
}

bool StateMachine::init()
{
    _log::behav << "(StateMachine::init)" << std::endl;
    if (currentState_)
        return false;

    currentState_=initState_;

    context_->aspectEngine->executeFunction(currentState_->initFunctions());
}

const State * StateMachine::newState(const std::string & id)
{
    if (stateMap_[id])
        return NULL;

    return stateMap_[id]=new State(id);
}

const State * StateMachine::state(const std::string & id)
{
    return stateMap_[id];
}

const State * StateMachine::initState()
{
     return initState_;
}

const State * StateMachine::currentState()
{
     return currentState_;
}



bool StateMachine::makeTransition(const State * newState)
{
    assert(newState);

    if (currentState_==newState)
        return false;

    currentState_=const_cast<State *>(newState);

    _log::behav<<"(StateMachine::makeTransition) Making the transition to the state "<<currentState_->id()<<std::endl;

    context_->aspectEngine->executeFunction(currentState_->initFunctions());


    return true;
}

const StateMachine::StateList * StateMachine::states()
{
    StateList * result= new StateList();

    StateMap::iterator it= stateMap_.begin();

    for (;it!=stateMap_.end();it++)
    {
        result->push_back(it->second);
    }

    return result;
}

void StateMachine::clear()
{
    StateMap::iterator it= stateMap_.begin();

    //if the state machine hasnt been initalized yet or if it is at the init state, return
    if ((!currentState_) || ( currentState_==initState_))
        return;

    for (;it!=stateMap_.end();it++)
    {
        if (it->second!=currentState_)
            delete it->second;
    }

    stateMap_.clear();
    initState_->clear();
    currentState_->clear();
}


bool StateMachine::checkConsistency()
{

    if ((!currentState_) || (currentState_==initState_) || (stateMap_[currentState_->id()]))
        return true;
    else
        return false;
}


}
}
}

