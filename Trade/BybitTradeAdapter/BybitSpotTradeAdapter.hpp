#ifndef __BYBITSPOTTRADEADAPTER_H__
#define __BYBITSPOTTRADEADAPTER_H__

#include "BybitSignTool.hpp"
#include "RestRequester.hpp"
#include "TimeUtils.hpp"
#include "TradeNode.hpp"
#include "TradeAdapter.hpp"
#include "Logger.hpp"

#include <map>
#include <string>

#include <nlohmann/json.hpp>

using namespace QuantCrypto::Trade;
using namespace Util::Sign;

namespace QuantCrypto::Trade::Bybit {

class BybitSpotTradeHandler : public QuantCrypto::Trade::SpotTradeNode
{
public:
    BybitSpotTradeHandler(const nlohmann::json &config)
        : config_(config)
        , apiSecret_(config["exchange"]["bybit"]["apiSecret"].get<std::string>())
        , logger_("BybitSpotTrade")
    {
    }

    bool createOrder(Order *order)
    {
        static const std::string Path = "spot/v1/order";
        static std::map<std::string, std::string> params{
            {"api_key", config_["exchange"]["bybit"]["apiKey"].get<std::string>()}};
        
        return Util::Requester::RestRequester::post(
            [this, &order]() {
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
                
                return request;
            },
            []() {
                return "application/x-www-form-urlencoded";
            },
            []() {
                return "";
            },
            [](const auto &response) {
                if (response.code != 200) {
                    return false;
                }
                const nlohmann::json result = nlohmann::json::parse(response.body);
                if (result["ret_code"].get<int>() != 0 || result["ext_code"].get<std::string>() != "") {
                    return false;
                }
                return true;
            });
    }

    bool deleteOrder(Order *order)
    {
        static const std::string Path = "spot/v1/order";
        static std::map<std::string, std::string> params{
            {"api_key", config_["exchange"]["bybit"]["apiKey"].get<std::string>()}};

        return Util::Requester::RestRequester::del(
            [this, &order]() {
                params["orderLinkId"] = order->customOrderId_;
                params["timestamp"] = std::to_string(Util::Time::getTime());

                const auto signQueryString = BybitSignTool::signHttpReq(params, apiSecret_);
                const auto request = URL + Path + "?" + signQueryString;

                return request;
            },
            [](const auto &response) {
                if (response.code != 200) {
                    return false;
                }
                const nlohmann::json result = nlohmann::json::parse(response.body);
                if (result["ret_code"].get<int>() != 0 || result["ext_code"].get<std::string>() != "") {
                    return false;
                }
                return true;
            });
    }

    bool queryWallet()
    {
        static const std::string Path = "spot/v1/account";
        static std::map<std::string, std::string> params{
            {"api_key", config_["exchange"]["bybit"]["apiKey"].get<std::string>()}};

        nlohmann::json respBody;
        if (!Util::Requester::RestRequester::get(
                [this]() {
                    params["timestamp"] = std::to_string(Util::Time::getTime());

                    const auto signQueryString = BybitSignTool::signHttpReq(params, apiSecret_);
                    const auto request = URL + Path + "?" + signQueryString;

                    return request;
                },
                [&respBody](const auto &response) {
                    if (response.code != 200) {
                        return false;
                    }
                    const nlohmann::json result = nlohmann::json::parse(response.body);
                    if (result["ret_code"].get<int>() != 0 || result["ext_code"].get<std::string>() != "") {
                        return false;
                    }
                    respBody = result;
                    return true;
                })) {
            return false;
        }

        const auto balances = respBody["result"]["balances"];
        for (const auto &coinObj : balances) {
            const std::string coin = coinObj["coin"].get<std::string>();
            auto &coinPosition = wallet_[coin];

            coinPosition.total_ = std::stod(coinObj["total"].get<std::string>());
            coinPosition.free_ = std::stod(coinObj["free"].get<std::string>());
            coinPosition.locked_ = std::stod(coinObj["locked"].get<std::string>());

            logger_.info("coin={}, position={}", coin, coinPosition.dump());
        }

        return true;
    }

private:
    static inline std::string genOrderId(const std::string &symbol)
    {
        static long long sequenceNo = 0;
        return std::string("Bybit") + Util::Time::getDateTime() + symbol + std::to_string(sequenceNo++);
    }

    static inline std::string URL = "https://api.bybit.com/";
    nlohmann::json config_;
    std::string apiSecret_;
    Util::Log::Logger logger_;
};

using BybitSpotTradeAdapter = TradeAdapter<Bybit::BybitSpotTradeHandler, nlohmann::json>;

} // namespace QuantCrypto::Trade::Bybit
#endif // __BYBITSPOTTRADEADAPTER_H__