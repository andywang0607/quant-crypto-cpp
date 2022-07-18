#ifndef __BYBITPERPETUALTRADEADAPTER_H__
#define __BYBITPERPETUALTRADEADAPTER_H__

#include "BybitSignTool.hpp"
#include "TimeUtils.hpp"
#include "TradeApi.hpp"

#include <map>
#include <string>

#include <nlohmann/json.hpp>
#include <restclient-cpp/restclient.h>
#include <spdlog/spdlog.h>

using namespace QuantCrypto::Trade;
using namespace Util::Sign;

namespace QuantCrypto::Trade::Bybit {

class BybitPerpetualTradeAdapter : public QuantCrypto::Trade::TradeApi
{
public:
    BybitPerpetualTradeAdapter(const nlohmann::json &config)
        : config_(config)
        , apiSecret_(config["exchange"]["bybit"]["apiSecret"].get<std::string>())
    {
    }

    virtual bool createOrder(Order *order) override
    {
        static const std::string Path = "private/linear/order/create";

        static std::map<std::string, std::string> params{
            {"api_key", config_["exchange"]["bybit"]["apiKey"].get<std::string>()}};

        auto *perPetualOrder = dynamic_cast<PerpetualOrder *>(order);
        params["symbol"] = perPetualOrder->symbol_;
        params["qty"] = std::to_string(perPetualOrder->qty_);
        params["price"] = std::to_string(perPetualOrder->price_);
        params["timestamp"] = std::to_string(Util::Time::getTime());
        params["side"] = [perPetualOrder]() -> std::string {
            if (perPetualOrder->side_ == Side::Buy) {
                return "Buy";
            }
            return "Sell";
        }();
        params["order_type"] = [&perPetualOrder]() {
            if (perPetualOrder->type_ == OrderType::Limit) {
                return "Limit";
            }
            return "Market";
        }();
        params["time_in_force"] = [perPetualOrder]() {
            if (perPetualOrder->timeInForce_ == TimeinForce::GTC) {
                return "GoodTillCancel";
            }
            if (perPetualOrder->timeInForce_ == TimeinForce::FOK) {
                return "FillOrKill";
            }
            return "ImmediateOrCancel";
        }();
        perPetualOrder->customOrderId_ = genOrderId(perPetualOrder->symbol_);
        params["orderLinkId"] = perPetualOrder->customOrderId_;

        params["reduce_only"] = perPetualOrder->reduceOnly_ ? "True" : "False";
        params["close_on_trigger"] = perPetualOrder->closeOnTrigger_ ? "True" : "False";
        if (perPetualOrder->takeProfitPrice_ > 0) {
            params["take_profit"] = std::to_string(perPetualOrder->takeProfitPrice_);
        }
        if (perPetualOrder->stopLossPrice_ > 0) {
            params["stop_loss"] = std::to_string(perPetualOrder->stopLossPrice_);
        }
        params["tp_trigger_by"] = [perPetualOrder]() {
            if (perPetualOrder->takeProfitTriggerType_ == TriggerPriceType::LastPrice) {
                return "LastPrice";
            }
            if (perPetualOrder->takeProfitTriggerType_ == TriggerPriceType::IndexPrice) {
                return "IndexPrice";
            }
            return "MarkPrice";
        }();
        params["sl_trigger_by"] = [perPetualOrder]() {
            if (perPetualOrder->stopLossTriggerType_ == TriggerPriceType::LastPrice) {
                return "LastPrice";
            }
            if (perPetualOrder->stopLossTriggerType_ == TriggerPriceType::IndexPrice) {
                return "IndexPrice";
            }
            return "MarkPrice";
        }();
        if (perPetualOrder->positionIndex_ != PositionIndex::Default) {
            params["position_idx"] = static_cast<std::underlying_type_t<PositionIndex>>(perPetualOrder->positionIndex_);
        }

        const auto signQueryString = BybitSignTool::signHttpReq(params, apiSecret_);
        const auto request = URL + Path + "?" + signQueryString;

        RestClient::Response r = RestClient::post(request, "application/x-www-form-urlencoded", "");
        if (r.code != 200) {
            spdlog::info("[BybitPerpetualTrade] createOrder failed, request={} r.code={}, r.body={}", request, r.code, r.body);
            return false;
        }

        const auto result = nlohmann::json::parse(r.body);
        if (result["ret_code"].get<int>() != 0 || result["ext_code"].get<std::string>() != "") {
            spdlog::info("[BybitPerpetualTrade] createOrder failed, request={} r.code={}, r.body={}", request, r.code, r.body);
            return false;
        }

        return true;
    }

    virtual bool deleteOrder(Order *order) override
    {
        static const std::string Path = "private/linear/order/cancel";
        static std::map<std::string, std::string> params{
            {"api_key", config_["exchange"]["bybit"]["apiKey"].get<std::string>()}};

        params["order_link_id"] = order->customOrderId_;
        params["symbol"] = order->symbol_;
        params["timestamp"] = std::to_string(Util::Time::getTime());

        const auto signQueryString = BybitSignTool::signHttpReq(params, apiSecret_);
        const auto request = URL + Path + "?" + signQueryString;

        RestClient::Response r = RestClient::post(request, "application/x-www-form-urlencoded", "");
        if (r.code != 200) {
            spdlog::info("[BybitSpotTrade] deleteOrder failed, request={} r.code={}, r.body={}", request, r.code, r.body);
            return false;
        }

        const auto result = nlohmann::json::parse(r.body);
        if (result["ret_code"].get<int>() != 0 || result["ext_code"].get<std::string>() != "") {
            spdlog::info("[BybitPerpetualTrade] deleteOrder failed, request={} r.code={}, r.body={}", request, r.code, r.body);
            return false;
        }

        return true;
    }

    virtual bool queryWallet() override
    {
        static const std::string Path = "v2/private/wallet/balance";
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

        const auto result = nlohmann::json::parse(r.body);
        if (result["ret_code"].get<int>() != 0 || result["ext_code"].get<std::string>() != "") {
            spdlog::info("[BybitPerpetualTrade] queryWallet failed, request={} r.code={}, r.body={}", request, r.code, r.body);
            return false;
        }

        return true;
    }

private:
    inline std::string genOrderId(const std::string &symbol)
    {
        static long long sequenceNo = 0;
        return std::string("BybitP") + Util::Time::getDateTime() + symbol + std::to_string(sequenceNo++);
    }

    static inline std::string URL = "https://api.bybit.com/";
    nlohmann::json config_;
    std::string apiSecret_;
};

} // namespace QuantCrypto::Trade::Bybit

#endif // __BYBITPERPETUALTRADEADAPTER_H__