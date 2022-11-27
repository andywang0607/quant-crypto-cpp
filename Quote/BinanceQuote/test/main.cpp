#include "BinanceSpotQuote.hpp"
#include "BinanceContractQuote.hpp"
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
    config["exchange"]["binance"]["contract"] = bybitConfig;

    BinanceSpotQuoteAdapter binanceSpot(config);
    BinanceContractQuoteAdapter binanceContract(config);

    while (true) {
    }
    return 0;
}