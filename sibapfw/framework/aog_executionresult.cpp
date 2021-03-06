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

#include "aog_executionresult.h"

namespace csu {
namespace aog {
namespace framework {

ExecutionResult::ExecutionResult()
{
}



bool ExecutionResult::assertResult()
{
    return assertResult_;
}

void ExecutionResult::assertResult(bool res)
{
    assertResult_=res;
}

const std::string & ExecutionResult::functionCalled()
{
    return functionCalled_;
}

void ExecutionResult::functionCalled(const std::string & f)
{
    functionCalled_=f;
}

bool ExecutionResult::functionResult()
{
    return functionResult_;
}

void ExecutionResult::functionResult(bool result )
{
    functionResult_=result;
}

const std::string &  ExecutionResult::actionWidget()
{
    return actionWidget_;
}

void  ExecutionResult::actionWidget(const std::string &  w)
{
    actionWidget_=w;
}

const std::string & ExecutionResult::actionEvent()
{
    return actionEvent_;
}

void ExecutionResult::actionEvent(const std::string &  e)
{
    actionEvent_=e;
}

const std::string & ExecutionResult::currentState()
{
    return currentState_;
}

void ExecutionResult::currentState(const std::string &  s)
{
    currentState_=s;
}

}
}
}
