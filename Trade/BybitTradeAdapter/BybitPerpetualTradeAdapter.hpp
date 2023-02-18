#ifndef __BYBITPERPETUALTRADEADAPTER_H__
#define __BYBITPERPETUALTRADEADAPTER_H__

#include "BybitSignTool.hpp"
#include "Logger.hpp"
#include "RestRequester.hpp"
#include "TimeUtils.hpp"
#include "TradeAdapter.hpp"
#include "TradeNode.hpp"

#include <map>
#include <string>

#include <nlohmann/json.hpp>

using namespace QuantCrypto::Trade;
using namespace Util::Sign;

namespace QuantCrypto::Trade::Bybit {

class BybitPerpetualTradeHandler : public QuantCrypto::Trade::PerpetualTradeNode
{
public:
    BybitPerpetualTradeHandler(const nlohmann::json &config)
        : config_(config)
        , apiSecret_(config["exchange"]["bybit"]["apiSecret"].get<std::string>())
        , logger_("BybitPerpetualTrade")
    {
    }

    bool createOrder(Order *order)
    {
        static const std::string Path = "private/linear/order/create";

        static std::map<std::string, std::string> params{
            {"api_key", config_["exchange"]["bybit"]["apiKey"].get<std::string>()}};

        return Util::Requester::RestRequester::post(
            [this, &order]() {
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
                params["order_type"] = [perPetualOrder]() {
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
                params["order_link_id"] = perPetualOrder->customOrderId_;

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
        static const std::string Path = "private/linear/order/cancel";
        static std::map<std::string, std::string> params{
            {"api_key", config_["exchange"]["bybit"]["apiKey"].get<std::string>()}};

        return Util::Requester::RestRequester::post(
            [this, &order]() {
                params["order_link_id"] = order->customOrderId_;
                params["symbol"] = order->symbol_;
                params["timestamp"] = std::to_string(Util::Time::getTime());

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

    bool queryWallet()
    {
        static const std::string Path = "v2/private/wallet/balance";
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

        for (const auto &[coin, coinObj] : respBody["result"].items()) {
            auto &coinPosition = wallet_[coin];

            coinPosition.equity_ = coinObj["equity"].get<double>();
            coinPosition.availableBalance_ = coinObj["available_balance"].get<double>();
            coinPosition.usedMargin_ = coinObj["used_margin"].get<double>();
            coinPosition.orderMargin_ = coinObj["order_margin"].get<double>();
            coinPosition.positionMargin_ = coinObj["position_margin"].get<double>();
            coinPosition.occClosingFee_ = coinObj["occ_closing_fee"].get<double>();
            coinPosition.occFundingFee_ = coinObj["occ_funding_fee"].get<double>();
            coinPosition.walletBalance_ = coinObj["wallet_balance"].get<double>();
            coinPosition.realisedPnl_ = coinObj["realised_pnl"].get<double>();
            coinPosition.unrealisedPnl_ = coinObj["unrealised_pnl"].get<double>();
            coinPosition.accumulatedPnl_ = coinObj["cum_realised_pnl"].get<double>();

            logger_.info("coin={}, position={}", coin, coinPosition.dump());
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
    Util::Log::Logger logger_;
};

using BybitPerpetualTradeAdapter = TradeAdapter<Bybit::BybitPerpetualTradeHandler, nlohmann::json>;

} // namespace QuantCrypto::Trade::Bybit

#endif // __BYBITPERPETUALTRADEADAPTER_H__