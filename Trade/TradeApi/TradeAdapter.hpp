#ifndef __TRADEADAPTER_H__
#define __TRADEADAPTER_H__

#include "TradeStruct.hpp"

namespace QuantCrypto::Trade {

template<typename TradeHandler, typename ConfigType>
class TradeAdapter
{
public:
    TradeAdapter(const ConfigType &config)
        : handler_(config)
    {
    }

    bool createOrder(Order *order)
    {
        return handler_.createOrder(order);
    }

    bool deleteOrder(Order *order)
    {
        return handler_.deleteOrder(order);
    }

    bool queryWallet()
    {
        return handler_.queryWallet();
    }

    auto *allocateOrder(std::string symbol, double price, double qty, Side side)
    {
        return handler_.allocateOrder(symbol, price, qty, side);
    }

    template<typename OrderType>
    void recycleOrder(OrderType *order)
    {
        return handler_.recycleOrder(order);
    }

    auto &getHandler() const
    {
        return handler_;
    }

private:
    TradeHandler handler_;
};
}

#endif // __TRADEADAPTER_H__