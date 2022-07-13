#ifndef __TRADEAPI_H__
#define __TRADEAPI_H__

#include "TradeStruct.hpp"

namespace QuantCrypto::Trade {

class TradeApi
{
public:
    virtual bool createOrder(Order *order) = 0;
    virtual bool deleteOrder(Order *order) = 0; // CustomOrderId
    virtual bool queryPosition() = 0;
};

} // namespace QuantCrypto::Trade

#endif // __TRADEAPI_H__