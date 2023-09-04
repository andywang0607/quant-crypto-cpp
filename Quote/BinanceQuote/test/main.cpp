#include "BinanceSpotQuote.hpp"
#include "BinanceContractQuote.hpp"
#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace QuantCrypto::Quote;

int main()
{
    nlohmann::json config;
    config["enabled"] = true;
    config["symbol"].push_back("BTCUSDT");
    config["symbol"].push_back("ETHUSDT");
    config["klineType"] = nlohmann::json::array({"1m", "1h"});
    config["market"] = "spot";

    BinanceSpotQuoteAdapter binanceSpot(config);

    config["market"] = "contract";
    BinanceContractQuoteAdapter binanceContract(config);

    while (true) {
    }
    return 0;
}