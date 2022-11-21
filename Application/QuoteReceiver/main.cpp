#include "BybitSpotQuote.hpp"
#include "BybitPerpetualQuote.hpp"
#include "BinanceSpotQuote.hpp"
#include "QuoteWritter.hpp"

#include <fstream>
#include <memory>

#include <nlohmann/json.hpp>

using namespace QuantCrypto::Quote;
using namespace QuantCrypto::QuoteReceiver::QuoteUtil;

int main(int argc, char *argv[])
{
    std::ifstream ifs(argv[1]);

    nlohmann::json config = nlohmann::json::parse(ifs);

    BybitSpotQuoteAdapter bybitSpot(config);
    BybitPerpetualQuoteAdapter bybitPerpetual(config);
    BinanceSpotQuoteAdapter binanceSpot(config);

    auto bookWritter = std::make_shared<QuoteWritter<MarketBook>>(config);
    bybitSpot.addNewSymbolEvent<MarketBook>([bookWritter](auto &symbol, auto &quote) {
        bookWritter->init(symbol, quote);
    });
    bybitPerpetual.addNewSymbolEvent<MarketBook>([bookWritter](auto &symbol, auto &quote) {
        bookWritter->init(symbol, quote);
    });
    binanceSpot.addNewSymbolEvent<MarketBook>([bookWritter](auto &symbol, auto &quote) {
        bookWritter->init(symbol, quote);
    });

    auto tradeWritter = std::make_shared<QuoteWritter<Trade>>(config);
    bybitSpot.addNewSymbolEvent<Trade>([tradeWritter](auto &symbol, auto &quote) {
        tradeWritter->init(symbol, quote);
    });
    bybitPerpetual.addNewSymbolEvent<Trade>([tradeWritter](auto &symbol, auto &quote) {
        tradeWritter->init(symbol, quote);
    });
    binanceSpot.addNewSymbolEvent<Trade>([tradeWritter](auto &symbol, auto &quote) {
        tradeWritter->init(symbol, quote);
    });

    auto klineWritter = std::make_shared<QuoteWritter<Kline>>(config);
    bybitSpot.addNewSymbolEvent<Kline>([klineWritter](auto &symbol, auto &quote) {
        klineWritter->init(symbol, quote);
    });
    bybitPerpetual.addNewSymbolEvent<Kline>([klineWritter](auto &symbol, auto &quote) {
        klineWritter->init(symbol, quote);
    });
    binanceSpot.addNewSymbolEvent<Kline>([klineWritter](auto &symbol, auto &quote) {
        klineWritter->init(symbol, quote);
    });

    while (true) {
    }
    return 0;
}