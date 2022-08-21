#ifndef __QUOTEAPI_H__
#define __QUOTEAPI_H__

#include "QuoteData.hpp"
#include "Topic.hpp"

namespace QuantCrypto::Quote {

class QuoteApi
{
public:
    static inline Util::Event::Topic<MarketBook &> onNewBook;
    static inline Util::Event::Topic<Trade &> onNewTrade;
    static inline Util::Event::Topic<Kline &> onNewKline;
    static inline Util::Event::Topic<InstrumentInfo &> onNewInstrumentInfo;
};
}
#endif // __QUOTEAPI_H__