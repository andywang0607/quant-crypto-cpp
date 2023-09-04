#include "BybitSpotQuote.hpp"
#include "BybitContractQuote.hpp"
#include "BaseNode.hpp"

using namespace QuantCrypto::Quote;
using namespace Util::DynamicLoader;

extern "C" BaseNode *constructor(const nlohmann::json &config)
{
    auto market = config["market"].get<std::string>();
    if (market== "spot") {
        return new BybitSpotQuoteAdapter(config);
    } 
    if (market == "contract") {
        return new BybitContractQuoteAdapter(config);
    } 
    throw std::runtime_error("Unknown market type");  
}

extern "C" void destructor(BaseNode *node)
{
    delete node;
}