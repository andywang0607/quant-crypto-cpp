#ifndef __TRADENODE_H__
#define __TRADENODE_H__

#include "TradeStruct.hpp"
#include "FixedObjectPool.hpp"

#include <unordered_map>

namespace QuantCrypto::Trade {
class SpotTradeNode
{
public:
    SpotTradeNode()
        : orderPool_(128)
    {
    }

    virtual ~SpotTradeNode() = default;

    Order *allocateOrder(std::string symbol, double price, double qty, Side side)
    {
        return orderPool_.construct(symbol, price, qty, side);
    }

    void recycleOrder(Order *order)
    {
        orderPool_.recycle(order);
    }

protected:
    bool init()
    {
        return queryWallet();
    }

    virtual bool queryWallet() { return false; };

    Util::Resource::FixedObjectPool<Order> orderPool_;

    using WalletType = std::unordered_map<std::string, SpotPosition>;
    WalletType wallet_;
};

} // namespace QuantCrypto::Trade
#endif // __TRADENODE_H__