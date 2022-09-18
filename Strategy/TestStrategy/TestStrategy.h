#ifndef __TESTSTRATEGY_H__
#define __TESTSTRATEGY_H__

#include "StrategyBase.h"
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace QuantCrypto::Strategy {

class TestStrategy : public StrategyBase
{
public:
    TestStrategy(const nlohmann::json &config)
        : StrategyBase(config)
    {
    }

    ~TestStrategy()
    {
    }

    virtual void start() override
    {
        spdlog::info("TestStrategy start");
    }

    virtual void stop() override
    {
        spdlog::info("TestStrategy stop");
    }

    virtual void onNewBook(const ExchangeT &exchange, const std::string &symbol) override
    {
        spdlog::info("TestStrategy onNewBook, exchange={}, symbol={}", exchange, symbol);
    }

    virtual void onNewTrade(const ExchangeT &exchange, const std::string &symbol) override
    {
        spdlog::info("TestStrategy onNewTrade, exchange={}, symbol={}", exchange, symbol);
    }

    virtual void onNewKline(const ExchangeT &exchange, const std::string &symbol) override
    {
        spdlog::info("TestStrategy onNewKline, exchange={}, symbol={}", exchange, symbol);
    }
};
}
#endif // __TESTSTRATEGY_H__