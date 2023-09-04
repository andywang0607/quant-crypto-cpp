#include "QuoteReceiver.h"

using namespace QuantCrypto::Quote;
using namespace QuantCrypto::QuoteReceiver;
using namespace Util::DynamicLoader;

extern "C" BaseNode *constructor(const nlohmann::json &config)
{
    return new QuoteReceiver(config);
}

extern "C" void destructor(BaseNode *node)
{
    delete node;
}