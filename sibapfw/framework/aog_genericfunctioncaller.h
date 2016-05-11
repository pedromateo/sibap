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
