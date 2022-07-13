#include "BybitSpotQuote.hpp"
#include "BybitPerpetualQuote.hpp"

#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

using namespace QuantCrypto::Quote;

int main(int argc, char *argv[])
{
    std::ifstream ifs(argv[1]);

    nlohmann::json config = nlohmann::json::parse(ifs);

    BybitSpotQuoteAdapter bybitSpot(config);
    bybitSpot.connect();

    BybitPerpetualQuoteAdapter bybitPerpetual(config);
    bybitPerpetual.connect();

    while (true) {
    }
    return 0;
}