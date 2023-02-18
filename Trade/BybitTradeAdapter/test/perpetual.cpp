#include "BybitPerpetualTradeAdapter.hpp"
#include <iostream>

#include <nlohmann/json.hpp>

using namespace QuantCrypto::Trade;

int main()
{
    nlohmann::json config;

    nlohmann::json bybitConfig;
    bybitConfig["exchange"]["bybit"]["apiKey"] = "";
    bybitConfig["exchange"]["bybit"]["apiSecret"] = "";

    auto bybitTradeAdapter = Bybit::BybitPerpetualTradeAdapter(bybitConfig);
    auto mockOrder = PerpetualOrder("BTCUSDT", 21000, 0.001, Side::Buy);
    mockOrder.timeInForce_ = TimeinForce::GTC;

    auto ret = bybitTradeAdapter.createOrder(&mockOrder);
    std::cout << "createOrder test, ret = " << ret << "\n";
    std::cout << "mockOrder.customOrderId_ = " << mockOrder.customOrderId_ << "\n";

    ret = bybitTradeAdapter.deleteOrder(&mockOrder);
    std::cout << "deleteOrder test, ret = " << ret << "\n";

    ret = bybitTradeAdapter.queryWallet();
    std::cout << "queryPosition test, ret = " << ret << "\n";
    for (const auto &[symbol, position] : bybitTradeAdapter.getHandler().wallet_) {
        std::cout << "symbol=" << symbol << "/"
                  << "position=" << position.availableBalance_ << "\n";
    }
    return 0;
}