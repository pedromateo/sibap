#include <framework/aog_forward.h>

#include <set>
#include <boost/function.hpp>

namespace csu {
namespace aog {
namespace framework {

//FIXME Si funciona, quitar esto de aqui y ponerlo en forward
typedef std::list<std::string> FunctionList;
typedef std::set<std::string> FunctionSet;

class BehaviorEngineBase
{
 virtual const FunctionSet * loadScriptsinEngine(const StringList * scripts)=0;
 virtual bool callFunction(const std::string & func)=0;
 virtual bool callFunction(const FunctionSet & funcList)=0;
 virtual void registerFunction(const std::string &state,boost::function <bool(void)> transitionFunc)=0;

};

}
}
}
