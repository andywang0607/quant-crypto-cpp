#ifndef __QUOTEADAPTER_H__
#define __QUOTEADAPTER_H__

#include "WebSocketReceiver.hpp"
#include "BaseNode.hpp"
#include <utility>

#include <nlohmann/json.hpp>

namespace QuantCrypto::Quote {

template <typename QuoteHandlerType>
class QuoteAdapter : public Util::DynamicLoader::BaseNode, public Util::Websocket::WebSocketReceiver<QuoteHandlerType>
{
public:
    explicit QuoteAdapter(const nlohmann::json &config)
        : Util::DynamicLoader::BaseNode(config)
        , Util::Websocket::WebSocketReceiver<QuoteHandlerType>(config)
    {
    }

    template <typename QuoteType, typename F>
    void addNewSymbolEvent(F &&f)
    {
        this->getHandler().template addNewSymbolEvent<QuoteType>(std::forward<F>(f));
    }
};
} // namespace QuantCrypto::Quote
#endif // __QUOTEADAPTER_H__