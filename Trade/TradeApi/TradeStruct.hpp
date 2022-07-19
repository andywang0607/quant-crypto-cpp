#ifndef __TRADESTRUCT_H__
#define __TRADESTRUCT_H__

#include <string>
#include <utility>

#include <spdlog/fmt/fmt.h>

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
    virtual ~Order() = default;

    std::string symbol_;
    double price_;
    double qty_;
    Side side_;
    OrderType type_;
    TimeinForce timeInForce_;
    std::string customOrderId_;
};

enum class TriggerPriceType
{
    LastPrice,
    IndexPrice,
    MarkPrice,
};

enum class PositionIndex
{
    Default = -1,
    OneWay = 0,
    BuyOfBothSide = 1,
    SellOfBothSide = 2,
};

struct PerpetualOrder : public Order
{
    PerpetualOrder(std::string symbol, double price, double qty, Side side)
        : Order(std::move(symbol), price, qty, side)
        , reduceOnly_(false)
        , closeOnTrigger_(false)
        , takeProfitPrice_(-1.0)
        , stopLossPrice_(-1.0)
        , takeProfitTriggerType_(TriggerPriceType::LastPrice)
        , stopLossTriggerType_(TriggerPriceType::LastPrice)
        , positionIndex_(PositionIndex::Default)
    {
    }
    virtual ~PerpetualOrder() = default;

    bool reduceOnly_;
    bool closeOnTrigger_;
    double takeProfitPrice_;
    double stopLossPrice_;
    TriggerPriceType takeProfitTriggerType_;
    TriggerPriceType stopLossTriggerType_;
    PositionIndex positionIndex_; 
};

struct SpotPosition
{
    SpotPosition() = default;
    ~SpotPosition() = default;

    double total_;
    double free_;
    double locked_;

    inline std::string dump() const
    {
        return fmt::format("total={}, free={}, locked={}",
                           total_, free_, locked_);
    }
};

struct PerpetualPosition
{
    PerpetualPosition() = default;
    ~PerpetualPosition() = default;

    double equity_;
    double availableBalance_;
    double usedMargin_;
    double orderMargin_;
    double positionMargin_;
    double occClosingFee_;
    double occFundingFee_;
    double walletBalance_;
    double realisedPnl_;
    double unrealisedPnl_;
    double accumulatedPnl_;

    inline std::string dump() const
    {
        return fmt::format("equity={}, availableBalance={}, usedMargin={}, orderMargin={}, positionMargin={}, occClosingFee={}, occFundingFee={}, walletBalance={}, realisedPnl={}, unrealisedPnl={}, accumulatedPnl={}",
                           equity_, availableBalance_, usedMargin_, orderMargin_, positionMargin_, occClosingFee_, occFundingFee_, walletBalance_, realisedPnl_, unrealisedPnl_, accumulatedPnl_);
    }
};

} // namespace QuantCrypto::Trade
#endif // __TRADESTRUCT_H__