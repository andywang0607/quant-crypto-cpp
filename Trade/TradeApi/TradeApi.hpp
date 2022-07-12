#ifndef __TRADEAPI_H__
#define __TRADEAPI_H__

#include <atomic>
#include <string>
#include <utility>

namespace QuantCrypto::Trade {

enum class Side
{
    Buy,
    Sell,
};

enum class OrderType
{
    Limit,
    Market,
    LimitMaker,
};

enum class TimeinForce
{
    GTC, // Good Till Canceled
    FOK, // Fill or Kill
    IOC, // Immediate or Cancel
};

struct Order
{
    Order(std::string symbol, double price, double qty, Side side)
        : symbol_(std::move(symbol))
        , price_(price)
        , qty_(qty)
        , side_(side)
        , type_(OrderType::Limit)
        , timeInForce_(TimeinForce::IOC)
    {
    }

    std::string symbol_;
    double price_;
    double qty_;
    Side side_;
    OrderType type_;
    TimeinForce timeInForce_;
    std::string customOrderId_;
};

class TradeApi
{
public:
    virtual bool createOrder(Order &order) = 0;
    virtual bool deleteOrder(const std::string &orderId) = 0; // CustomOrderId
    virtual bool queryPosition() = 0;
};

} // namespace QuantCrypto::Trade

#endif // __TRADEAPI_H__