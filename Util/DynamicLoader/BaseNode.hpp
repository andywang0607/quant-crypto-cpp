#ifndef __BASENODE_H__
#define __BASENODE_H__

#include "Logger.hpp"

namespace Util::DynamicLoader {

class BaseNode
{
public:
    template <typename... ConstructorArgs>
    BaseNode(ConstructorArgs... args)
    {
    }

    virtual ~BaseNode()
    {
    }

    virtual void start() {};
    virtual void stop() {};

    Util::Log::Logger logger_;
};

}

#endif // __BASENODE_H__