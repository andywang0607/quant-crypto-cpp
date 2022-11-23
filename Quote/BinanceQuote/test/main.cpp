#include "BinanceSpotQuote.hpp"
#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace QuantCrypto::Quote;

int main()
{
    nlohmann::json config;

    nlohmann::json bybitConfig;
    bybitConfig["enabled"] = true;
    bybitConfig["symbol"].push_back("BTCUSDT");
    bybitConfig["symbol"].push_back("ETHUSDT");
    bybitConfig["klineType"] = nlohmann::json::array({"1m", "1h"});
    config["exchange"]["binance"]["spot"] = bybitConfig;


    BinanceSpotQuoteAdapter binanceSpot(config);


    while (true) {
    }
    return 0;
}