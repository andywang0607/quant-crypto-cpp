#ifndef __BASENODELOADER_H__
#define __BASENODELOADER_H__

#include "DynamicLoader.hpp"
#include <nlohmann/json.hpp>

namespace Util::DynamicLoader {
class BaseNodeLoader
{
public:
    BaseNodeLoader(const nlohmann::json &config)
        : loader_(config["soName"].get<std::string>(), config["constructor"].get<std::string>(), config["destructor"].get<std::string>())
    {
        node_ = loader_.newObject(config);
    }

    ~BaseNodeLoader()
    {
        if (node_) {
            loader_.deleteObject(node_);
            node_ = nullptr;
        }
    }

    BaseNode *getNode() const
    {
        return node_;
    }

private:
    DynamicLoader<const nlohmann::json &> loader_;
    BaseNode *node_;
};

}

#endif // __BASENODELOADER_H__