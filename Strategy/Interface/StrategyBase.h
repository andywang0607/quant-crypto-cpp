#ifndef __STRATEGYBASE_H__
#define __STRATEGYBASE_H__

#include "QuoteApi.hpp"
#include "StrategyInterface.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <unordered_set>

namespace QuantCrypto::Strategy {

class StrategyBase : public StrategyInterface
{
public:
    StrategyBase(const nlohmann::json &config)
    {
        // subscribe symbol format: $EXCHANGE@$SYMBOL
        auto &subscribeSymbolArr = config["subscribeSymbol"];
        for (const auto &subscibeSymbol : subscribeSymbolArr) {
            updateSubscribeSymbolMap(subscibeSymbol);
        }

        subscribeNewBookCB([this](const auto &exchange, const auto &book) {
            if (!isSubscribeSymbol(exchange, book.header_)) {
                return;
            }
            onNewBook(book);
        });

        subscribeNewTradeCB([this](const auto &exchange, const auto &trade) {
            if (!isSubscribeSymbol(exchange, trade.header_)) {
                return;
            }
            onNewTrade(trade);
        });

        subscribeNewKlineCB([this](const auto &exchange, const auto &kline) {
            if (!isSubscribeSymbol(exchange, kline.header_)) {
                return;
            }
            onNewKline(kline);
        });
    }

    virtual ~StrategyBase()
    {
    }

protected:
    Util::Event::Unsubscriber unsubscriber_;

    template <typename Func>
    void subscribeNewBookCB(Func &&cb)
    {
        unsubscriber_ += Quote::QuoteApi::onNewBook.subscribe(std::forward<Func>(cb));
    }

    template <typename Func>
    void subscribeNewTradeCB(Func &&cb)
    {
        unsubscriber_ += Quote::QuoteApi::onNewTrade.subscribe(std::forward<Func>(cb));
    }

    template <typename Func>
    void subscribeNewKlineCB(Func &&cb)
    {
        unsubscriber_ += Quote::QuoteApi::onNewKline.subscribe(std::forward<Func>(cb));
    }

private:
    inline void updateSubscribeSymbolMap(const std::string &symbolWithExchange)
    {
        const auto delimiterIdx = symbolWithExchange.find("@");
        const std::string exchange = symbolWithExchange.substr(0, delimiterIdx);
        const std::string symbol = symbolWithExchange.substr(delimiterIdx + 1);

        subscribeSymbolMap_[convert2ExchangeT(exchange)].insert(symbol);
    }

    static inline Quote::ExchangeT convert2ExchangeT(const std::string &exchangeStr)
    {
        if (strcasecmp(exchangeStr.c_str(), "Bybit") == 0) {
            return Quote::ExchangeT::ByBit;
        }
        return Quote::ExchangeT::ByBit;
    }

    inline bool isSubscribeSymbol(const Quote::ExchangeT &exchange, const Quote::Header &header)
    {
        const auto &subscribeSymbolSet = subscribeSymbolMap_[exchange];

        return subscribeSymbolSet.find(header.symbol_) != subscribeSymbolSet.end();
    }

    std::unordered_map<Quote::ExchangeT, std::unordered_set<std::string>> subscribeSymbolMap_;
};
} // namespace QuantCrypto::Strategy
#endif // __STRATEGYBASE_H__