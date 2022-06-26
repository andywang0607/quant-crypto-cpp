#include "BybitSpotQuote.hpp"

#include <iostream>

#include <nlohmann/json.hpp>

using namespace QuantCrypto::Quote;

int main()
{
    nlohmann::json config;

    nlohmann::json bybitConfig;
    bybitConfig["enabled"] = true;
    bybitConfig["symbol"].push_back("BTCUSDT");
    bybitConfig["symbol"].push_back("ETHUSDT");
    config["exchange"]["bybit"] = bybitConfig;

    BybitSpotQuoteAdapter bybit(config);
    bybit.connect();

    while (true) {
    }
    return 0;
}