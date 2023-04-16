#ifndef __BYBITCONTRACTQUOTE_H__
#define __BYBITCONTRACTQUOTE_H__

#include "BybitV5QuoteHandler.hpp"
#include "QuoteAdapter.hpp"

#include <chrono>
#include <cstddef>

namespace QuantCrypto::Quote {

class BybitContractQuoteHandler : public BybitV5QuoteHandler
{
public:
    static inline const std::string Uri = "wss://stream.bybit.com/v5/public/linear";

    explicit BybitContractQuoteHandler(const nlohmann::json &config)
        : BybitV5QuoteHandler(config["exchange"]["bybit"]["contract"])
    {
    }

    void onMessage(const std::string &msg)
    {
        BybitV5QuoteHandler::onMessage<MarketT::USDTPerpetual>(msg);
    }
};

using BybitContractQuoteAdapter = QuoteAdapter<BybitContractQuoteHandler>;

} // namespace QuantCrypto::Quote

#endif // __BYBITCONTRACTQUOTE_H__