#include "BybitSpotTradeAdapter.hpp"
#include <iostream>

#include <nlohmann/json.hpp>

using namespace QuantCrypto::Trade;

int main()
{
    nlohmann::json config;

    nlohmann::json bybitConfig;
    bybitConfig["exchange"]["bybit"]["apiKey"] = "";
    bybitConfig["exchange"]["bybit"]["apiSecret"] = "";

    auto bybitTradeAdapter = Bybit::BybitSpotTradeAdapter(bybitConfig);
    auto *mockOrder = bybitTradeAdapter.allocateOrder("ETHUSDT", 10, 1, Side::Buy);
    mockOrder->timeInForce_ = TimeinForce::GTC;

    auto ret = bybitTradeAdapter.createOrder(mockOrder);
    std::cout << "createOrder test, ret = " << ret << "\n";
    std::cout << "mockOrder->customOrderId_ = " << mockOrder->customOrderId_ << "\n";

    ret = bybitTradeAdapter.deleteOrder(mockOrder);
    std::cout << "deleteOrder test, ret = " << ret << "\n";

    bybitTradeAdapter.recycleOrder(mockOrder);

    return 0;
}