#ifndef __QUOTENODE_H__
#define __QUOTENODE_H__

#include "QuoteData.hpp"

#include <string>
#include <unordered_map>

namespace QuantCrypto::Quote {
class QuoteNode
{
public:
    std::unordered_map<std::string, MarketBook> marketBook_;
    std::unordered_map<std::string, Trade> trade_;
};

} // namespace QuantCrypto::Quote
#endif // __QUOTENODE_H__