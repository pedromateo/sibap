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

#ifndef AOG_STATEMACHINE_H
#define AOG_STATEMACHINE_H

#include <framework/aog_forward.h>
#include <string>
#include <map>

namespace csu {
namespace aog {
namespace framework {

class State
{
public:
    State(const std::string & id);
    const std::string &id();
    void addInitFunc(const std::string & func);
    void addInitFunc(const FunctionSet & funcList);
    void addActionFunc(const std::string & func);
    void addActionFunc(const FunctionSet & funcList);
    const FunctionSet & initFunctions();
    const FunctionSet & actionFunctions();
    void clear();

private:
    std::string id_;
    FunctionSet initFuncList_;
    FunctionSet funcList_;
};

class StateMachine
{

public:
    typedef std::list<State *> StateList;
public:
    StateMachine(BehaviorConfiguration* context);
    /// creates and return a new state with id as parameter
    const State * newState(const std::string & id);
    /// make the transition to a new state
    bool makeTransition(const State * newState);
    /// Get the state object from its id
    const State * state(const std::string & id);
    /// Get the states objects
    const StateList * states();
    /// Init the State Machine
    bool init();
    /// Return the initial State
    const State * initState();
    /// Return the current State
    const State * currentState();
    bool checkConsistency();
    /// Delete all states and functions different from the current state
    void clear();

private:
    BehaviorConfiguration* context_;
    State * currentState_,* initState_;
    typedef std::map<std::string,State *> StateMap;
    StateMap stateMap_;
};

}
}
}
#endif // AOG_STATEMACHINE_H
