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

#include "aog_genericelementregistrar.h"

namespace csu {
namespace aog {
namespace framework {


GenericElementRegistrar::GenericElementRegistrar(BehaviorConfiguration *context):context_(context)
{}

GenericElementRegistrar::~GenericElementRegistrar()
{}

bool GenericElementRegistrar::isRegistrable(const std::string name)
{
    return registerSet_.find(name) != registerSet_.end();
}

void GenericElementRegistrar::registerClass(const std::string name)
{
    registerSet_.insert(name);
}

}
}
}
