#ifndef __BYBITSPOTTRADEADAPTER_H__
#define __BYBITSPOTTRADEADAPTER_H__

#include "BybitSignTool.hpp"
#include "TimeUtils.hpp"
#include "TradeApi.hpp"
#include "TradeNode.hpp"

#include <map>
#include <string>

#include <nlohmann/json.hpp>
#include <restclient-cpp/restclient.h>
#include <spdlog/spdlog.h>

using namespace QuantCrypto::Trade;
using namespace Util::Sign;

namespace QuantCrypto::Trade::Bybit {

class BybitSpotTradeAdapter : public QuantCrypto::Trade::TradeApi, public QuantCrypto::Trade::SpotTradeNode
{
public:
    BybitSpotTradeAdapter(const nlohmann::json &config)
        : config_(config)
        , apiSecret_(config["exchange"]["bybit"]["apiSecret"].get<std::string>())
    {
    }

    virtual bool createOrder(Order *order) override
    {
        static const std::string Path = "spot/v1/order";

        static std::map<std::string, std::string> params{
            {"api_key", config_["exchange"]["bybit"]["apiKey"].get<std::string>()}};

        params["symbol"] = order->symbol_;
        params["qty"] = std::to_string(order->qty_);
        params["price"] = std::to_string(order->price_);
        params["timestamp"] = std::to_string(Util::Time::getTime());
        params["side"] = [order]() -> std::string {
            if (order->side_ == Side::Buy) {
                return "BUY";
            }
            return "SELL";
        }();
        params["type"] = [&order]() {
            if (order->type_ == OrderType::Limit) {
                return "LIMIT";
            }
            if (order->type_ == OrderType::Market) {
                return "MARKET";
            }
            return "LIMIT_MAKER";
        }();
        params["timeInForce"] = [order]() {
            if (order->timeInForce_ == TimeinForce::GTC) {
                return "GTC";
            }
            if (order->timeInForce_ == TimeinForce::FOK) {
                return "FOK";
            }
            return "IOC";
        }();
        order->customOrderId_ = genOrderId(order->symbol_);
        params["orderLinkId"] = order->customOrderId_;

        const auto signQueryString = BybitSignTool::signHttpReq(params, apiSecret_);
        const auto request = URL + Path + "?" + signQueryString;

        RestClient::Response r = RestClient::post(request, "application/x-www-form-urlencoded", "");
        if (r.code != 200) {
            spdlog::info("[BybitSpotTrade] createOrder failed, request={} r.code={}, r.body={}", request, r.code, r.body);
            return false;
        }

        return true;
    }

    virtual bool deleteOrder(Order *order) override
    {
        static const std::string Path = "spot/v1/order";
        static std::map<std::string, std::string> params{
            {"api_key", config_["exchange"]["bybit"]["apiKey"].get<std::string>()}};

        params["orderLinkId"] = order->customOrderId_;
        params["timestamp"] = std::to_string(Util::Time::getTime());

        const auto signQueryString = BybitSignTool::signHttpReq(params, apiSecret_);
        const auto request = URL + Path + "?" + signQueryString;

        RestClient::Response r = RestClient::del(request);
        if (r.code != 200) {
            spdlog::info("[BybitSpotTrade] deleteOrder failed, request={} r.code={}, r.body={}", request, r.code, r.body);
            return false;
        }

        return true;
    }

    virtual bool queryWallet() override
    {
        static const std::string Path = "spot/v1/account";
        static std::map<std::string, std::string> params{
            {"api_key", config_["exchange"]["bybit"]["apiKey"].get<std::string>()}};

        params["timestamp"] = std::to_string(Util::Time::getTime());

        const auto signQueryString = BybitSignTool::signHttpReq(params, apiSecret_);
        const auto request = URL + Path + "?" + signQueryString;

        RestClient::Response r = RestClient::get(request);
        if (r.code != 200) {
            spdlog::info("[BybitSpotTrade] queryWallet failed, request={} r.code={}, r.body={}", request, r.code, r.body);
            return false;
        }

        const auto result = nlohmann::json::parse(r.body)["result"]["balances"];
        for (const auto &coinObj : result) {
            const std::string coin = coinObj["coin"].get<std::string>();
            auto &coinPosition = wallet_[coin];

            coinPosition.total_ = std::stod(coinObj["total"].get<std::string>());
            coinPosition.free_ = std::stod(coinObj["free"].get<std::string>());
            coinPosition.locked_ = std::stod(coinObj["locked"].get<std::string>());

            spdlog::info("[BybitSpotTrade] coin={}, position={}", coin, coinPosition.dump());
        }

        return true;
    }

private:
    inline std::string genOrderId(const std::string &symbol)
    {
        static long long sequenceNo = 0;
        return std::string("Bybit") + Util::Time::getDateTime() + symbol + std::to_string(sequenceNo++);
    }

    static inline std::string URL = "https://api.bybit.com/";
    nlohmann::json config_;
    std::string apiSecret_;
};

} // namespace QuantCrypto::Trade::Bybit
#endif // __BYBITSPOTTRADEADAPTER_H__