#include "BaseNodeLoader.hpp"
#include "TestStrategy.h"
#include "BybitSpotQuote.hpp"
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>

using namespace QuantCrypto::Strategy;
using namespace QuantCrypto::Quote;

int main()
{
    nlohmann::json config;

    auto strategyArr = nlohmann::json::array();
    nlohmann::json strategy{
        {"soName", "libTestStrategy.so"},
        {"constructor", "constructor"},
        {"destructor", "destructor"},
        {"strategyName", "Test"},
        {"subscribeSymbol", nlohmann::json::array({"Bybit@BTCUSDT"})}};
    strategyArr.push_back(strategy);

    config["strategy"]["list"] = strategyArr;

    std::vector<Util::DynamicLoader::BaseNodeLoader> loaders;
    for (const auto &strategyConfig : config["strategy"]["list"]) {
        loaders.emplace_back(strategyConfig);
    }

    nlohmann::json bybitConfig;
    bybitConfig["enabled"] = true;
    bybitConfig["symbol"].push_back("BTCUSDT");
    bybitConfig["symbol"].push_back("ETHUSDT");
    bybitConfig["klineType"] = nlohmann::json::array({"1m", "1h"});
    config["exchange"]["bybit"]["spot"] = bybitConfig;

    BybitSpotQuoteAdapter bybitSpot(config);
    for (const auto &loader : loaders) {
        loader.getNode()->start();
    }

    while(true){}

    for (const auto &loader : loaders) {
        loader.getNode()->stop();
    }
    return 0;
}