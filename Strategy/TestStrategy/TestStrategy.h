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

    virtual void onNewBook(const Quote::MarketBook &book) override
    {
        spdlog::info("TestStrategy onNewBook, exchange={}, symbol={}", book.header_.source_, book.header_.symbol_);
    }

    virtual void onNewTrade(const Quote::Trade &trade) override
    {
        spdlog::info("TestStrategy onNewTrade, exchange={}, symbol={}", trade.header_.source_, trade.header_.symbol_);
    }

    virtual void onNewKline(const Quote::Kline &kline) override
    {
        spdlog::info("TestStrategy onNewKline, exchange={}, symbol={}", kline.header_.source_, kline.header_.symbol_);
    }
};
}
#endif // __TESTSTRATEGY_H__