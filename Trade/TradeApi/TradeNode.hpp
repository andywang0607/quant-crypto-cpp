#ifndef __TRADENODE_H__
#define __TRADENODE_H__

#include "TradeStruct.hpp"

#include <unordered_map>

namespace QuantCrypto::Trade {
class SpotTradeNode
{
public:
    SpotTradeNode() = default;
    virtual ~SpotTradeNode() = default;
    std::unordered_map<std::string, SpotPosition> wallet_;
};

class PerpetualTradeNode
{
public:
    PerpetualTradeNode() = default;
    virtual ~PerpetualTradeNode() = default;
    std::unordered_map<std::string, PerpetualPosition> wallet_;
};

} // namespace QuantCrypto::Trade
#endif // __TRADENODE_H__