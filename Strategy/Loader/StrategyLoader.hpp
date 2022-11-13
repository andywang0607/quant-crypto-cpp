#ifndef __STRATEGYLOADER_H__
#define __STRATEGYLOADER_H__

#include "DynamicStrategyLoader.hpp"
#include <nlohmann/json.hpp>

namespace QuantCrypto::Strategy {

class StrategyLoader
{
public:
    StrategyLoader(const nlohmann::json &config)
        : loader_(config["soName"].get<std::string>(), config["constructor"].get<std::string>(), config["destructor"].get<std::string>())
    {
        strategy_ = loader_.newObject(config);
    }

    ~StrategyLoader()
    {
        if (strategy_) {
            loader_.deleteObject(strategy_);
            strategy_ = nullptr;
        }
    }

    StrategyBase *getStrategy() const
    {
        return strategy_;
    }

private:
    DynamicStrategyLoader<const nlohmann::json &> loader_;
    StrategyBase *strategy_;
};

}
#endif // __STRATEGYLOADER_H__