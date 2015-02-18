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

#ifndef AOG_EXECUTIONRESULT_H
#define AOG_EXECUTIONRESULT_H

#include <framework/aog_forward.h>

namespace csu {
namespace aog {
namespace framework {

class ExecutionResult
{
public:
    ExecutionResult();

    bool assertResult();
    void assertResult(bool);

    const std::string & functionCalled();
    void functionCalled(const std::string & );

    bool functionResult();
    void functionResult(bool result );

    const std::string &  actionWidget();
    void  actionWidget(const std::string &  );

    const std::string & actionEvent();
    void actionEvent(const std::string &  );

    const std::string & currentState();
    void currentState(const std::string &  );

private:
    bool assertResult_;
    std::string functionCalled_;
    bool functionResult_;
    std::string actionWidget_;
    std::string actionEvent_;
    std::string currentState_;

};

}
}
}
#endif // AOG_EXECUTIONRESULT_H
