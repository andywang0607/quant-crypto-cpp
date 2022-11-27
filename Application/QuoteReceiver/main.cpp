#include "BybitSpotQuote.hpp"
#include "BybitPerpetualQuote.hpp"
#include "BinanceSpotQuote.hpp"
#include "BinanceContractQuote.hpp"
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
    BinanceContractQuoteAdapter binanceContract(config);

    auto bookWritter = std::make_shared<QuoteWritter<MarketBook>>(config);
    bybitSpot.addNewSymbolEvent<MarketBook>([bookWritter](auto &symbol, auto &quote) {
        bookWritter->init(symbol, quote.header_.source_);
    });
    bybitPerpetual.addNewSymbolEvent<MarketBook>([bookWritter](auto &symbol, auto &quote) {
        bookWritter->init(symbol, quote.header_.source_);
    });
    binanceSpot.addNewSymbolEvent<MarketBook>([bookWritter](auto &symbol, auto &quote) {
        bookWritter->init(symbol, quote.header_.source_);
    });
    binanceContract.addNewSymbolEvent<MarketBook>([bookWritter](auto &symbol, auto &quote) {
        bookWritter->init(symbol, quote.header_.source_);
    });

    auto tradeWritter = std::make_shared<QuoteWritter<Trade>>(config);
    bybitSpot.addNewSymbolEvent<Trade>([tradeWritter](auto &symbol, auto &quote) {
        tradeWritter->init(symbol, quote.header_.source_);
    });
    bybitPerpetual.addNewSymbolEvent<Trade>([tradeWritter](auto &symbol, auto &quote) {
        tradeWritter->init(symbol, quote.header_.source_);
    });
    binanceSpot.addNewSymbolEvent<Trade>([tradeWritter](auto &symbol, auto &quote) {
        tradeWritter->init(symbol, quote.header_.source_);
    });
    binanceContract.addNewSymbolEvent<Trade>([tradeWritter](auto &symbol, auto &quote) {
        tradeWritter->init(symbol, quote.header_.source_);
    });

    auto klineWritter = std::make_shared<QuoteWritter<Kline>>(config);
    bybitSpot.addNewSymbolEvent<Kline>([klineWritter](auto &symbol, auto &quote) {
        klineWritter->init(symbol, quote.header_.source_);
    });
    bybitPerpetual.addNewSymbolEvent<Kline>([klineWritter](auto &symbol, auto &quote) {
        klineWritter->init(symbol, quote.header_.source_);
    });
    binanceSpot.addNewSymbolEvent<Kline>([klineWritter](auto &symbol, auto &quote) {
        klineWritter->init(symbol, quote.header_.source_);
    });
    binanceContract.addNewSymbolEvent<Kline>([klineWritter](auto &symbol, auto &quote) {
        klineWritter->init(symbol, quote.header_.source_);
    });

    while (true) {
    }
    return 0;
}