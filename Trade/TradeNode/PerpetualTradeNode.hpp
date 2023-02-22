#ifndef __TRADENODE_H__
#define __TRADENODE_H__

#include "TradeStruct.hpp"
#include "FixedObjectPool.hpp"

#include <unordered_map>

namespace QuantCrypto::Trade {
class PerpetualTradeNode
{
public:
    PerpetualTradeNode()
        : orderPool_(128)
    {
    }
    virtual ~PerpetualTradeNode() = default;

    PerpetualOrder *allocateOrder(std::string symbol, double price, double qty, Side side)
    {
        return orderPool_.construct(symbol, price, qty, side);
    }

    void recycleOrder(PerpetualOrder *order)
    {
        orderPool_.recycle(order);
    }

protected:
    bool init()
    {
        return queryWallet();
    }

    virtual bool queryWallet() { return false; };

    Util::Resource::FixedObjectPool<PerpetualOrder> orderPool_;

    using WalletType = std::unordered_map<std::string, PerpetualPosition>;
    WalletType wallet_;
};

} // namespace QuantCrypto::Trade
#endif // __TRADENODE_H__