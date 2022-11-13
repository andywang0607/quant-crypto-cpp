#ifndef __STRATEGYINTERFACE_H__
#define __STRATEGYINTERFACE_H__

#include "QuoteData.hpp"

namespace QuantCrypto::Strategy {

class StrategyInterface
{
public:
    virtual void start() = 0;
    virtual void stop() = 0;

    virtual void onNewBook(const QuantCrypto::Quote::ExchangeT &exchange, const std::string &symbol) = 0;
    virtual void onNewTrade(const QuantCrypto::Quote::ExchangeT &exchange, const std::string &symbol) = 0;
    virtual void onNewKline(const QuantCrypto::Quote::ExchangeT &exchange, const std::string &symbol) = 0;
};
} // namespace QuantCrypto::Strategy

#endif // __STRATEGYINTERFACE_H__