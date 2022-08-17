#ifndef __QUOTENODE_H__
#define __QUOTENODE_H__

#include "QuoteData.hpp"
#include "QuoteApi.hpp"
#include "PublisherMap.hpp"

#include <string>
#include <unordered_map>

namespace QuantCrypto::Quote {

class QuoteNode
{
public:
    QuoteNode() = default;
    ~QuoteNode() = default;

    template <typename QuoteType, typename MessageType, typename F, typename = std::enable_if_t<std::is_invocable_v<F, const MessageType &, QuoteType &>>>
    void forQuoteData(const std::string &symbol, const MessageType &msg, F &&f)
    {
        if constexpr (std::is_same_v<QuoteType, MarketBook>) {
            auto iter = marketBook_.try_emplace(symbol).first;
            std::forward<F>(f)(msg, iter->second);

            QuoteApi::subscribeBook(iter->second);
        }
        if constexpr (std::is_same_v<QuoteType, Trade>) {
            auto iter = trade_.try_emplace(symbol).first;
            std::forward<F>(f)(msg, iter->second);

            QuoteApi::subscribeTrade(iter->second);
        }
        if constexpr (std::is_same_v<QuoteType, Kline>) {
            auto iter = kline_.try_emplace(symbol).first;
            std::forward<F>(f)(msg, iter->second);

            QuoteApi::subscribeKline(iter->second);
        }
        if constexpr (std::is_same_v<QuoteType, InstrumentInfo>) {
            auto iter = instrumentInfo_.try_emplace(symbol).first;
            std::forward<F>(f)(msg, iter->second);

            QuoteApi::subscribeInstrumentInfo(iter->second);
        }
    }

    template <typename QuoteType, typename F>
    void addNewSymbolEvent(F &&f)
    {
        if constexpr (std::is_same_v<QuoteType, MarketBook>) {
            unsubscriber_ += marketBook_.subscribe(std::forward<F>(f));
        }
        if constexpr (std::is_same_v<QuoteType, Trade>) {
            unsubscriber_ += trade_.subscribe(std::forward<F>(f));
        }
        if constexpr (std::is_same_v<QuoteType, Kline>) {
            unsubscriber_ += kline_.subscribe(std::forward<F>(f));
        }
        if constexpr (std::is_same_v<QuoteType, InstrumentInfo>) {
            unsubscriber_ += instrumentInfo_.subscribe(std::forward<F>(f));
        }
    }

    Util::Event::PublisherMap<std::string, MarketBook> marketBook_;
    Util::Event::PublisherMap<std::string, Trade> trade_;
    Util::Event::PublisherMap<std::string, Kline> kline_;
    Util::Event::PublisherMap<std::string, InstrumentInfo> instrumentInfo_;

    Util::Event::Unsubscriber unsubscriber_;
};

} // namespace QuantCrypto::Quote
#endif // __QUOTENODE_H__