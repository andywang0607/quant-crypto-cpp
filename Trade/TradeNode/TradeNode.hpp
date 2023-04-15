#ifndef __TRADENODE_H__
#define __TRADENODE_H__

#include "TradeStruct.hpp"
#include "FixedObjectPool.hpp"

#include <unordered_map>

namespace QuantCrypto::Trade {

template<typename OrderType, typename PositionType>
class TradeNode
{
public:
    TradeNode()
        : orderPool_(128)
    {
    }

    virtual ~TradeNode() = default;

    OrderType *allocateOrder(std::string symbol, double price, double qty, Side side)
    {
        return orderPool_.construct(symbol, price, qty, side);
    }

    void recycleOrder(OrderType *order)
    {
        orderPool_.recycle(order);
    }

protected:
    bool init()
    {
        return queryWallet();
    }

    virtual bool queryWallet() { return false; };

    Util::Resource::FixedObjectPool<OrderType> orderPool_;

    using WalletType = std::unordered_map<std::string, PositionType>;
    WalletType wallet_;
};

using SpotTradeNode = TradeNode<Trade::Order, Trade::SpotPosition>;
using PerpetualTradeNode = TradeNode<Trade::PerpetualOrder, Trade::PerpetualPosition>;


} // namespace QuantCrypto::Trade
#endif // __TRADENODE_H__