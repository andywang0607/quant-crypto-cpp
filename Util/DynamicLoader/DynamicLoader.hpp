#ifndef __DYNAMICLOADER_H__
#define __DYNAMICLOADER_H__

#include <dlfcn.h>
#include <unistd.h>
#include <string>

#include "BaseNode.hpp"

namespace Util::DynamicLoader {

template <typename... ConstructorArgs>
class DynamicLoader
{
    using ConstructorType = BaseNode *(*)(ConstructorArgs...);
    using DestructorType = void (*)(BaseNode *object);

public:
    DynamicLoader(const std::string &soName, const std::string &constructorName, const std::string &destructorName)
    {
        openLib(soName);
        constructor_ = (ConstructorType)findSymbol(constructorName);
        destructor_ = (DestructorType)findSymbol(destructorName);
    }

    ~DynamicLoader()
    {
        if (soHandle_) {
            dlclose(soHandle_);
            soHandle_ = nullptr;
        }
    }

    BaseNode *newObject(ConstructorArgs... args)
    {
        return constructor_(args...);
    }

    void deleteObject(BaseNode *object)
    {
        destructor_(object);
    }

private:
    void openLib(const std::string &soName)
    {
        dlerror();
        soHandle_ = dlopen(soName.c_str(), RTLD_LAZY);

        if (soHandle_ == nullptr) {
            throw std::runtime_error(dlerror());
        }
    }

    void *findSymbol(const std::string &name)
    {
        auto *ret = dlsym(soHandle_, name.c_str());
        auto *error = dlerror();

        if (error != nullptr) {
            throw std::runtime_error(error);
        }

        return ret;
    }

    ConstructorType constructor_;
    DestructorType destructor_;
    void *soHandle_ = nullptr;
};

}


#endif // __DYNAMICLOADER_H__