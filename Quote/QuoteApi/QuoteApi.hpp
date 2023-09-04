#ifndef __QUOTEAPI_H__
#define __QUOTEAPI_H__

#include "QuoteData.hpp"
#include "Topic.hpp"

namespace QuantCrypto::Quote {

class QuoteApi
{
public:
    static inline Util::Event::Topic<const ExchangeT &, MarketBook &> onNewBook;
    static inline Util::Event::Topic<const ExchangeT &, Trade &> onNewTrade;
    static inline Util::Event::Topic<const ExchangeT &, Kline &> onNewKline;
    static inline Util::Event::Topic<const ExchangeT &, InstrumentInfo &> onNewInstrumentInfo;

    static inline Util::Event::Topic<const std::string &, MarketBook &> onNewSymbolBook;
    static inline Util::Event::Topic<const std::string &, Trade &> onNewSymbolTrade;
    static inline Util::Event::Topic<const std::string &, Kline &> onNewSymbolKline;
    static inline Util::Event::Topic<const std::string &, InstrumentInfo &> onNewSymbolInstrumentInfo;
};
}
#endif // __QUOTEAPI_H__