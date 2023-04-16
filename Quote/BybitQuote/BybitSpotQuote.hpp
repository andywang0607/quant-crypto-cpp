#ifndef __BYBITQUOTE_H__
#define __BYBITQUOTE_H__

#include "BybitV5QuoteHandler.hpp"
#include "QuoteAdapter.hpp"

#include <cstddef>
#include <string>

namespace QuantCrypto::Quote {

class BybitSpotQuoteHandler : public BybitV5QuoteHandler
{
public:
    static inline const std::string Uri = "wss://stream.bybit.com/v5/public/spot";

    explicit BybitSpotQuoteHandler(const nlohmann::json &config)
        : BybitV5QuoteHandler(config["exchange"]["bybit"]["spot"])
    {
    }

    void onMessage(const std::string &msg)
    {
        BybitV5QuoteHandler::onMessage<MarketT::Spot>(msg);
    }
};

using BybitSpotQuoteAdapter = QuoteAdapter<BybitSpotQuoteHandler>;


} // namespace QuantCrypto::Quote
#endif // __BYBITQUOTE_H__