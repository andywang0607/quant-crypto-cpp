#include "TestStrategy.h"

using namespace QuantCrypto::Strategy;

extern "C" Util::DynamicLoader::BaseNode *constructor(const nlohmann::json &config)
{
    return new TestStrategy(config);
}

extern "C" void destructor(TestStrategy *node)
{
    delete node;
}