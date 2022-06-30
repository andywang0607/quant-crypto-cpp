#ifndef __QUOTENODE_H__
#define __QUOTENODE_H__

#include "QuoteData.hpp"

#include <string>
#include <unordered_map>
#include <chrono>

namespace QuantCrypto::Quote {
    
static long long getTime()
{
    const auto p1 = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(
               p1.time_since_epoch())
        .count();
}

class QuoteNode
{
public:
    std::unordered_map<std::string, MarketBook> marketBook_;
    std::unordered_map<std::string, Trade> trade_;
    std::unordered_map<std::string, Kline> kline_;
    std::unordered_map<std::string, InstrumentInfo> instrumentInfo_;
};

} // namespace QuantCrypto::Quote
#endif // __QUOTENODE_H__