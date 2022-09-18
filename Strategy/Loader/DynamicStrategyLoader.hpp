#ifndef __DYNAMICSTRATEGYLOADER_H__
#define __DYNAMICSTRATEGYLOADER_H__

#include "StrategyBase.h"

#include <dlfcn.h>
#include <unistd.h>
#include <string>


namespace QuantCrypto::Strategy {

template <typename... ConstructorArgs>
class DynamicStrategyLoader
{
    using ConstructorType = StrategyBase *(*)(ConstructorArgs...);
    using DestructorType = void (*)(StrategyBase *object);

public:
    DynamicStrategyLoader(const std::string &soName, const std::string &constructorName, const std::string &destructorName)
    {
        openLib(soName);
        constructor_ = (ConstructorType)findSymbol(constructorName);
        destructor_ = (DestructorType)findSymbol(destructorName);
    }

    ~DynamicStrategyLoader()
    {
        if (soHandle_) {
            dlclose(soHandle_);
            soHandle_ = nullptr;
        }
    }

    StrategyBase *newObject(ConstructorArgs... args)
    {
        return constructor_(args...);
    }

    void deleteObject(StrategyBase *object)
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

} // namespace QuantCrypto::Strategy

#endif // __DYNAMICSTRATEGYLOADER_H__
