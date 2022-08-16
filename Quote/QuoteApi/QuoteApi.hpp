#ifndef __QUOTEAPI_H__
#define __QUOTEAPI_H__

#include "QuoteData.hpp"
#include "Topic.hpp"

namespace QuantCrypto::Quote {

class QuoteApi
{
public:
    static inline Util::Event::Topic<MarketBook &> subscribeBook;
    static inline Util::Event::Topic<Trade &> subscribeTrade;
    static inline Util::Event::Topic<Kline &> subscribeKline;
    static inline Util::Event::Topic<InstrumentInfo &> subscribeInstrumentInfo;
};
}
#endif // __QUOTEAPI_H__