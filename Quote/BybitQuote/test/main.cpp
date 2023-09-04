#include "BybitSpotQuote.hpp"
#include "BybitContractQuote.hpp"
#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace QuantCrypto::Quote;

int main()
{
    nlohmann::json bybitConfig;
    bybitConfig["enabled"] = true;
    bybitConfig["symbol"].push_back("BTCUSDT");
    bybitConfig["symbol"].push_back("ETHUSDT");
    bybitConfig["klineType"] = nlohmann::json::array({"1m", "1h"});

    QuoteApi::onNewBook.subscribe([](auto &, auto &book){
        static int count = 0;
        spdlog::info("bookCount={}", count++);
    });

    QuoteApi::onNewTrade.subscribe([](auto &, auto &trade){
        static int count = 0;
        spdlog::info("tradeCount={}", count++);
    });

    QuoteApi::onNewKline.subscribe([](auto &, auto &trade){
        static int count = 0;
        spdlog::info("klineCount={}", count++);
    });

    QuoteApi::onNewInstrumentInfo.subscribe([](auto &, auto &trade){
        static int count = 0;
        spdlog::info("InstrumentInfoCount={}", count++);
    });

    bybitConfig["market"] = "spot";
    BybitSpotQuoteAdapter bybitSpot(bybitConfig);

    bybitSpot.addNewSymbolEvent<MarketBook>([](auto &symbol, auto &book){
        spdlog::info("BybitSpotQuoteAdapter NewSymbolEvent for book, symbol={}", symbol);
    });
    bybitSpot.addNewSymbolEvent<Trade>([](auto &symbol, auto &book){
        spdlog::info("BybitSpotQuoteAdapter NewSymbolEvent for Trade, symbol={}", symbol);
    });

    bybitConfig["market"] = "contract";
    BybitContractQuoteAdapter bybitPerpetual(bybitConfig);

    bybitPerpetual.addNewSymbolEvent<Trade>([](auto &symbol, auto &book){
        spdlog::info("BybitContractQuoteAdapter NewSymbolEvent for Trade, symbol={}", symbol);
    });

    while (true) {
    }
    return 0;
}