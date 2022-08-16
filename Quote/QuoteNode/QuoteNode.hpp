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
            auto result = marketBook_.find(symbol);
            if (result != marketBook_.end()) {
                std::forward<F>(f)(msg, result->second);
            } else {
                result = marketBook_.emplace(std::piecewise_construct, std::forward_as_tuple(symbol), std::forward_as_tuple()).first;
                std::forward<F>(f)(msg, result->second);
            }
            QuoteApi::subscribeBook(result->second);
        }
        if constexpr (std::is_same_v<QuoteType, Trade>) {
            auto result = trade_.find(symbol);
            if (result != trade_.end()) {
                std::forward<F>(f)(msg, result->second);
            } else {
                result = trade_.emplace(std::piecewise_construct, std::forward_as_tuple(symbol), std::forward_as_tuple()).first;
                std::forward<F>(f)(msg, result->second);
            }
            QuoteApi::subscribeTrade(result->second);
        }
        if constexpr (std::is_same_v<QuoteType, Kline>) {
            auto result = kline_.find(symbol);
            if (result != kline_.end()) {
                std::forward<F>(f)(msg, result->second);
            } else {
                result = kline_.emplace(std::piecewise_construct, std::forward_as_tuple(symbol), std::forward_as_tuple()).first;
                std::forward<F>(f)(msg, result->second);
            }
            QuoteApi::subscribeKline(result->second);
        }
        if constexpr (std::is_same_v<QuoteType, InstrumentInfo>) {
            auto result = instrumentInfo_.find(symbol);
            if (result != instrumentInfo_.end()) {
                std::forward<F>(f)(msg, result->second);
            } else {
                result = instrumentInfo_.emplace(std::piecewise_construct, std::forward_as_tuple(symbol), std::forward_as_tuple()).first;
                std::forward<F>(f)(msg, result->second);
            }
            QuoteApi::subscribeInstrumentInfo(result->second);
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