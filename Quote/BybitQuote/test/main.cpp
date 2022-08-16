#include "BybitSpotQuote.hpp"
#include "BybitPerpetualQuote.hpp"
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
    bybitConfig["klineType"] = nlohmann::json::array({"1m", "1h"});
    config["exchange"]["bybit"] = bybitConfig;

    QuoteApi::subscribeBook.subscribe([](auto &book){
        static int count = 0;
        spdlog::info("bookCount={}", count++);
    });

    QuoteApi::subscribeTrade.subscribe([](auto &trade){
        static int count = 0;
        spdlog::info("tradeCount={}", count++);
    });

    QuoteApi::subscribeKline.subscribe([](auto &trade){
        static int count = 0;
        spdlog::info("klineCount={}", count++);
    });

    QuoteApi::subscribeInstrumentInfo.subscribe([](auto &trade){
        static int count = 0;
        spdlog::info("InstrumentInfoCount={}", count++);
    });

    BybitSpotQuoteAdapter bybitSpot(config);
    bybitSpot.connect();

    BybitPerpetualQuoteAdapter bybitPerpetual(config);
    bybitPerpetual.connect();

    while (true) {
    }
    return 0;
}