#ifndef __STRATEGYINTERFACE_H__
#define __STRATEGYINTERFACE_H__

#include "QuoteData.hpp"

namespace QuantCrypto::Strategy {

class StrategyInterface
{
public:
    virtual void onStart() = 0;
    virtual void onStop() = 0;

    virtual void onNewBook(const QuantCrypto::Quote::MarketBook &book) = 0;
    virtual void onNewTrade(const QuantCrypto::Quote::Trade &trade) = 0;
    virtual void onNewKline(const QuantCrypto::Quote::Kline &kline) = 0;
};
} // namespace QuantCrypto::Strategy

#endif // __STRATEGYINTERFACE_H__