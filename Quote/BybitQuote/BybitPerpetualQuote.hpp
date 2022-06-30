#ifndef __BYBITPERPETUALQUOTE_H__
#define __BYBITPERPETUALQUOTE_H__

#include "QuoteData.hpp"
#include "QuoteNode.hpp"
#include "WebSocketReceiver.hpp"

#include <chrono>
#include <cstddef>
#include <string>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace QuantCrypto::Quote {

class BybitPerpetualQuoteHandler : public QuoteNode
{
public:
    static inline const std::string Uri = "wss://stream.bybit.com/realtime_public";

    explicit BybitPerpetualQuoteHandler(const nlohmann::json &config)
        : config_(config)
    {
    }

    std::vector<nlohmann::json> genSubscribeMsg()
    {
        std::vector<nlohmann::json> ret;

        const auto &symbols = config_["exchange"]["bybit"]["symbol"];

        for (const auto &symbol : symbols) {
            static nlohmann::json infoReq;

            infoReq["op"] = "subscribe";
            infoReq["args"] = nlohmann::json::array({fmt::format("instrument_info.100ms.{}", symbol)});

            ret.emplace_back(infoReq);
        }

        for (const auto &symbol : symbols) {
            static nlohmann::json tradeReq;

            tradeReq["op"] = "subscribe";
            tradeReq["args"] = nlohmann::json::array({fmt::format("trade.{}", symbol)});

            ret.emplace_back(tradeReq);
        }

        return ret;
    }

    void onMessage(const std::string &msg)
    {
        const auto jsonMsg = nlohmann::json::parse(msg);
        if (!jsonMsg.contains("topic")) {
            return;
        }
        const auto &topic = jsonMsg["topic"].get<std::string>();

        if (topic.find("trade") != std::string::npos) {
            handleTrade(jsonMsg);
            return;
        }

        if (topic.find("instrument_info") != std::string::npos) {
            handleInstrumentInfo(jsonMsg);
            return;
        }
    }

private:
    inline void handleTrade(const nlohmann::json &json)
    {
        if (!json.contains("data")) {
            return;
        }

        const auto &dataObj = json["data"][0];

        const std::string &symbol = dataObj["symbol"].get<std::string>();
        auto &trade = trade_[symbol];

        trade.header_.source_ = ExchangeT::ByBit;
        trade.header_.type_ = QuoteType::Trade;
        trade.header_.symbol_ = symbol;
        trade.header_.receivedTime_ = getTime();
        trade.header_.sourceTime_ = std::stoll(dataObj["trade_time_ms"].get<std::string>());

        trade.price_ = std::stod(dataObj["price"].get<std::string>());
        trade.qty_ = dataObj["size"].get<double>();
        trade.tradeType_ = dataObj["side"].get<std::string>() == "Sell" ? TradeType::Seller : TradeType::Buyer;
        trade.tradeId_ = dataObj["trade_id"].get<std::string>();

        spdlog::info("[Trade] {}", trade.dump());
    }

    inline void handleInstrumentInfo(const nlohmann::json &json)
    {
        if (!json.contains("data")) {
            return;
        }

        const auto symbol = [&json]() -> std::string {
            const auto topic = json["topic"].get<std::string>();
            auto found = topic.find_last_of(".");
            return topic.substr(found + 1);
        }();

        auto &instrumentInfo = instrumentInfo_[symbol];

        instrumentInfo.header_.source_ = ExchangeT::ByBit;
        instrumentInfo.header_.type_ = QuoteType::InstrumentInfo;
        instrumentInfo.header_.symbol_ = symbol;
        instrumentInfo.header_.receivedTime_ = getTime();
        instrumentInfo.header_.sourceTime_ = std::stoll(json["timestamp_e6"].get<std::string>());

        const auto dataType = json["type"].get<std::string>();
        if (dataType == "snapshot") {
            const auto &dataObj = json["data"];

            instrumentInfo.lastPrice_ = std::stod(dataObj["last_price"].get<std::string>());
            instrumentInfo.prevPrice24h_ = std::stod(dataObj["prev_price_24h"].get<std::string>());
            instrumentInfo.price24hPcnt_ = std::stod(dataObj["price_24h_pcnt_e6"].get<std::string>()) / static_cast<double>(1e6);
            instrumentInfo.highestPrice24h_ = std::stod(dataObj["high_price_24h"].get<std::string>());
            instrumentInfo.lowestPrice24h_ = std::stod(dataObj["low_price_24h"].get<std::string>());
            instrumentInfo.prevPrice1h_ = std::stod(dataObj["prev_price_1h"].get<std::string>());
            instrumentInfo.price1hPcnt_ = std::stod(dataObj["price_1h_pcnt_e6"].get<std::string>()) / static_cast<double>(1e6);
            instrumentInfo.markPrice_ = std::stod(dataObj["mark_price"].get<std::string>());
            instrumentInfo.indexPrice_ = std::stod(dataObj["index_price"].get<std::string>());
            instrumentInfo.openInterest_ = std::stod(dataObj["open_interest_e8"].get<std::string>()) / static_cast<double>(1e8);
            instrumentInfo.totalTurnover_ = std::stod(dataObj["total_turnover_e8"].get<std::string>()) / static_cast<double>(1e8);
            instrumentInfo.totalVolume_ = std::stod(dataObj["total_volume_e8"].get<std::string>()) / static_cast<double>(1e8);
            instrumentInfo.turnover24h_ = std::stod(dataObj["turnover_24h_e8"].get<std::string>()) / static_cast<double>(1e8);
            instrumentInfo.volume24h_ = std::stod(dataObj["volume_24h_e8"].get<std::string>()) / static_cast<double>(1e8);
            instrumentInfo.fundingRate_ = std::stod(dataObj["funding_rate_e6"].get<std::string>()) / static_cast<double>(1e6);
            instrumentInfo.predictFundingRate_ = std::stod(dataObj["predicted_funding_rate_e6"].get<std::string>()) / static_cast<double>(1e6);
            instrumentInfo.fundingRateInterval_ = std::stod(dataObj["funding_rate_interval"].get<std::string>());
            instrumentInfo.bestBidPrice_ = std::stod(dataObj["bid1_price"].get<std::string>());
            instrumentInfo.bestAskPrice_ = std::stod(dataObj["ask1_price"].get<std::string>());
            instrumentInfo.lastTickDirection_ = dataObj["last_tick_direction"].get<std::string>();
            instrumentInfo.nextFundingTime_ = toTimestamp(dataObj["next_funding_time"].get<std::string>());
        } else if (dataType == "delta") {
            const auto &dataObj = json["data"]["update"][0];

            instrumentInfo.lastPrice_ = dataObj.contains("last_price") ? std::stod(dataObj["last_price"].get<std::string>()) : instrumentInfo.lastPrice_;
            instrumentInfo.prevPrice24h_ = dataObj.contains("prev_price_24h") ? std::stod(dataObj["prev_price_24h"].get<std::string>()) : instrumentInfo.prevPrice24h_;
            instrumentInfo.price24hPcnt_ = dataObj.contains("price_24h_pcnt_e6") ? std::stod(dataObj["price_24h_pcnt_e6"].get<std::string>()) / static_cast<double>(1e6) : instrumentInfo.price24hPcnt_;
            instrumentInfo.highestPrice24h_ = dataObj.contains("high_price_24h") ? std::stod(dataObj["high_price_24h"].get<std::string>()) : instrumentInfo.highestPrice24h_;
            instrumentInfo.lowestPrice24h_ = dataObj.contains("low_price_24h") ? std::stod(dataObj["low_price_24h"].get<std::string>()) : instrumentInfo.lowestPrice24h_;
            instrumentInfo.prevPrice1h_ = dataObj.contains("prev_price_1h") ? std::stod(dataObj["prev_price_1h"].get<std::string>()) : instrumentInfo.prevPrice1h_;
            instrumentInfo.price1hPcnt_ = dataObj.contains("price_1h_pcnt_e6") ? std::stod(dataObj["price_1h_pcnt_e6"].get<std::string>()) / static_cast<double>(1e6) : instrumentInfo.price1hPcnt_;
            instrumentInfo.markPrice_ = dataObj.contains("mark_price") ? std::stod(dataObj["mark_price"].get<std::string>()) : instrumentInfo.markPrice_;
            instrumentInfo.indexPrice_ = dataObj.contains("index_price") ? std::stod(dataObj["index_price"].get<std::string>()) : instrumentInfo.indexPrice_;
            instrumentInfo.openInterest_ = dataObj.contains("open_interest_e8") ? std::stod(dataObj["open_interest_e8"].get<std::string>()) / static_cast<double>(1e8) : instrumentInfo.openInterest_;
            instrumentInfo.totalTurnover_ = dataObj.contains("total_turnover_e8") ? std::stod(dataObj["total_turnover_e8"].get<std::string>()) / static_cast<double>(1e8) : instrumentInfo.totalTurnover_;
            instrumentInfo.totalVolume_ = dataObj.contains("total_volume_e8") ? std::stod(dataObj["total_volume_e8"].get<std::string>()) / static_cast<double>(1e8) : instrumentInfo.totalVolume_;
            instrumentInfo.turnover24h_ = dataObj.contains("turnover_24h_e8") ? std::stod(dataObj["turnover_24h_e8"].get<std::string>()) / static_cast<double>(1e8) : instrumentInfo.turnover24h_;
            instrumentInfo.volume24h_ = dataObj.contains("volume_24h_e8") ? std::stod(dataObj["volume_24h_e8"].get<std::string>()) / static_cast<double>(1e8) : instrumentInfo.volume24h_;
            instrumentInfo.fundingRate_ = dataObj.contains("funding_rate_e6") ? std::stod(dataObj["funding_rate_e6"].get<std::string>()) / static_cast<double>(1e6) : instrumentInfo.fundingRate_;
            instrumentInfo.predictFundingRate_ = dataObj.contains("predicted_funding_rate_e6") ? std::stod(dataObj["predicted_funding_rate_e6"].get<std::string>()) / static_cast<double>(1e6) : instrumentInfo.predictFundingRate_;
            instrumentInfo.fundingRateInterval_ = dataObj.contains("funding_rate_interval") ? std::stod(dataObj["funding_rate_interval"].get<std::string>()) : instrumentInfo.fundingRateInterval_;
            instrumentInfo.bestBidPrice_ = dataObj.contains("bid1_price") ? std::stod(dataObj["bid1_price"].get<std::string>()) : instrumentInfo.bestBidPrice_;
            instrumentInfo.bestAskPrice_ = dataObj.contains("ask1_price") ? std::stod(dataObj["ask1_price"].get<std::string>()) : instrumentInfo.bestAskPrice_;
            instrumentInfo.lastTickDirection_ = dataObj.contains("last_tick_direction") ? dataObj["last_tick_direction"].get<std::string>() : instrumentInfo.lastTickDirection_;
            instrumentInfo.nextFundingTime_ = dataObj.contains("next_funding_time") ? toTimestamp(dataObj["next_funding_time"].get<std::string>()) : instrumentInfo.nextFundingTime_;
        }

        spdlog::info("[InstrumentInfo] {}", instrumentInfo.dump());
    }

    long long toTimestamp(const std::string &dateTime)
    {
        std::tm t{};
        std::istringstream ss(dateTime);

        ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
        if (ss.fail()) {
            throw std::runtime_error{"failed to parse time string"};
        }
        std::time_t time_stamp = mktime(&t);

        return time_stamp;
    }

    nlohmann::json config_;
};

using BybitPerpetualQuoteAdapter = WebSocketReceiver<BybitPerpetualQuoteHandler>;

} // namespace QuantCrypto::Quote

#endif // __BYBITPERPETUALQUOTE_H__