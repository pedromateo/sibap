#include "aog_genericfunctioncaller.h"

#include <iostream>

namespace csu {
namespace aog {
namespace framework {


GenericFunctionCaller::GenericFunctionCaller(BehaviorConfiguration *context):context_(context)
{
    std::cout<<"GenericFunctionCaller::constructor" << std::endl;
}

}
}
}
