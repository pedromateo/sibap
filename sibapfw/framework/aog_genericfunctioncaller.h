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

#ifndef AOG_GENERICFUNCTIONCALLER_H
#define AOG_GENERICFUNCTIONCALLER_H

#include <framework/aog_forward.h>

namespace csu {
namespace aog {
namespace framework {


class GenericFunctionCaller
{
public:
    GenericFunctionCaller(BehaviorConfiguration* context);
    virtual void initialize(void)=0;



protected:
    BehaviorConfiguration* context_;
};

}
}
}
#endif // AOG_GENERICFUNCTIONCALLER_H
