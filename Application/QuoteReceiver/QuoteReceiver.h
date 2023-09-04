#ifndef __QUOTERECEIVER_H__
#define __QUOTERECEIVER_H__

#include "QuoteApi.hpp"
#include "QuoteWritter.hpp"
#include "BaseNode.hpp"

#include <nlohmann/json.hpp>

namespace QuantCrypto::QuoteReceiver {

class QuoteReceiver : public Util::DynamicLoader::BaseNode
{
public:
    QuoteReceiver(const nlohmann::json &config)
        : Util::DynamicLoader::BaseNode(config)
        , bookWritter_(config["bookRoot"].get<std::string>())
        , tradeWritter_(config["tradeRoot"].get<std::string>())
        , klineWritter_(config["klineRoot"].get<std::string>())
    {
        unsubscriber_ += Quote::QuoteApi::onNewSymbolBook.subscribe([this](auto &symbol, auto &quote) {
            bookWritter_.init(symbol, quote.header_.source_);
        });
        unsubscriber_ += Quote::QuoteApi::onNewSymbolTrade.subscribe([this](auto &symbol, auto &quote) {
            tradeWritter_.init(symbol, quote.header_.source_);
        });
        unsubscriber_ += Quote::QuoteApi::onNewSymbolKline.subscribe([this](auto &symbol, auto &quote) {
            klineWritter_.init(symbol, quote.header_.source_);
        });
    }

    virtual ~QuoteReceiver() = default;

private:
    QuoteUtil::QuoteWritter<Quote::MarketBook> bookWritter_;
    QuoteUtil::QuoteWritter<Quote::Trade> tradeWritter_;
    QuoteUtil::QuoteWritter<Quote::Kline> klineWritter_;

    Util::Event::Unsubscriber unsubscriber_;
};

}

#endif // __QUOTERECEIVER_H__
