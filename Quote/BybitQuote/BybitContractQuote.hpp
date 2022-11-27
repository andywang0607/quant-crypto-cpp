#ifndef __BYBITCONTRACTQUOTE_H__
#define __BYBITCONTRACTQUOTE_H__

#include "QuoteAdapter.hpp"
#include "QuoteData.hpp"
#include "QuoteNode.hpp"
#include "TimeUtils.hpp"
#include "Logger.hpp"

#include <chrono>
#include <cstddef>
#include <string>

#include <nlohmann/json.hpp>

using namespace Util::Time;

namespace QuantCrypto::Quote {

class BybitContractQuoteHandler : public QuoteNode
{
public:
    static inline const std::string Uri = "wss://stream.bybit.com/realtime_public";

    explicit BybitContractQuoteHandler(const nlohmann::json &config)
        : config_(config["exchange"]["bybit"]["contract"])
        , logger_("BybitContractQuote")
    {
    }

    std::vector<nlohmann::json> genSubscribeMsg()
    {
        std::vector<nlohmann::json> ret;

        const auto &symbols = config_["symbol"];

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

        for (const auto &symbol : symbols) {
            static nlohmann::json bookReq;

            bookReq["op"] = "subscribe";
            bookReq["args"] = nlohmann::json::array({fmt::format("orderBookL2_25.{}", symbol)});

            ret.emplace_back(bookReq);
        }

        const auto &klineType = config_["klineType"];
        for (const auto &symbol : symbols) {
            static nlohmann::json klineReq;
            for (const auto &type : klineType) {
                static nlohmann::json klineReq;

                klineReq["op"] = "subscribe";
                klineReq["args"] = nlohmann::json::array({fmt::format("candle.{}.{}", type, symbol)});

                ret.emplace_back(klineReq);
            }
        }

        return ret;
    }

    void onMessage(const std::string &msg)
    {
        const auto jsonMsg = nlohmann::json::parse(msg);
        if (isPong(jsonMsg)) {
            logger_.info(msg);
        }
        if (!jsonMsg.contains("topic")) {
            return;
        }
        if (!jsonMsg.contains("data")) {
            return;
        }
        const auto &topic = jsonMsg["topic"].get<std::string>();

        if (topic.find("trade") != std::string::npos) {
            const auto &dataObj = jsonMsg["data"][0];
            const std::string &symbol = dataObj["symbol"].get<std::string>();

            forQuoteData<Trade>(symbol, ExchangeT::ByBit, dataObj, [this, symbol](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::Trade;
                updateHeader(quote, json, symbol, [&json]() {
                    return std::stoll(json["trade_time_ms"].get<std::string>());
                });
                updateTrade(quote, json);
                logger_.debug("[Trade] {}", quote.dump());
            });
            return;
        }

        if (topic.find("instrument_info") != std::string::npos) {
            const auto symbol = [&jsonMsg]() -> std::string {
                const auto topic = jsonMsg["topic"].get<std::string>();
                auto found = topic.find_last_of(".");
                return topic.substr(found + 1);
            }();
            forQuoteData<InstrumentInfo>(symbol, ExchangeT::ByBit, jsonMsg, [this, &symbol](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::InstrumentInfo;
                updateHeader(quote, json, symbol, [&json]() {
                    return std::stoll(json["timestamp_e6"].get<std::string>()) / 1000.f;
                });
                updateInstrumentInfo(quote, json);
                logger_.debug("[InstrumentInfo] {}", quote.dump());
            });
            return;
        }

        if (topic.find("orderBookL2_25") != std::string::npos) {
            const auto symbol = [&jsonMsg]() -> std::string {
                const auto topic = jsonMsg["topic"].get<std::string>();
                auto found = topic.find_last_of(".");
                return topic.substr(found + 1);
            }();
            forQuoteData<MarketBook>(symbol, ExchangeT::ByBit, jsonMsg, [this, &symbol](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::MarketBook;
                updateHeader(quote, json, symbol, [&json]() {
                    return std::stoll(json["timestamp_e6"].get<std::string>()) / 1000.f;
                });
                const auto isSnapshotBook = json["type"].get<std::string>() == "snapshot";
                updateMarketBook(quote, json, isSnapshotBook);
                logger_.debug("[MarketBook] {}", quote.dump());
            });
            return;
        }

        if (topic.find("candle") != std::string::npos) {
            const auto symbol = [&jsonMsg]() -> std::string {
                const auto topic = jsonMsg["topic"].get<std::string>();
                auto found = topic.find_last_of(".");
                return topic.substr(found + 1);
            }();

            forQuoteData<Kline>(symbol, ExchangeT::ByBit, jsonMsg, [this, symbol](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::Kline;
                updateHeader(quote, json, symbol, [&json]() {
                    return json["timestamp_e6"].get<long long>() / 1000;
                });
                updateKline(quote, json);
                logger_.debug("[Kline] {}", quote.dump());
            });
            return;
        }
    }

    static constexpr bool needPeriodicPing()
    {
        return true;
    }

    nlohmann::json genPingMessage()
    {
        return {
            {"op", "ping"}};
    }

    virtual bool enabled() override
    {
        return config_["enabled"].get<bool>();
    }

private:
    template <typename QuoteType, typename GetSourceTime>
    inline void updateHeader(QuoteType &quote, const nlohmann::json &json, const std::string &symbol, GetSourceTime &&f)
    {
        quote.header_.source_ = ExchangeT::ByBit;
        quote.header_.market_ = MarketT::USDTPerpetual;
        quote.header_.symbol_ = symbol;
        quote.header_.receivedTime_ = getTime();
        quote.header_.sourceTime_ = std::forward<GetSourceTime>(f)();
    }

    inline void updateTrade(Trade &quote, const nlohmann::json &json)
    {
        quote.price_ = std::stod(json["price"].get<std::string>());
        quote.qty_ = json["size"].get<double>();
        quote.tradeType_ = json["side"].get<std::string>() == "Sell" ? TradeType::Seller : TradeType::Buyer;
        quote.tradeId_ = json["trade_id"].get<std::string>();
    }

    inline void updateInstrumentInfo(InstrumentInfo &quote, const nlohmann::json &json)
    {
        const auto isSnapshot = json["type"].get<std::string>() == "snapshot";
        if (isSnapshot) {
            const auto &dataObj = json["data"];

            quote.lastPrice_ = std::stod(dataObj["last_price"].get<std::string>());
            quote.prevPrice24h_ = std::stod(dataObj["prev_price_24h"].get<std::string>());
            quote.price24hPcnt_ = std::stod(dataObj["price_24h_pcnt_e6"].get<std::string>()) / static_cast<double>(1e6);
            quote.highestPrice24h_ = std::stod(dataObj["high_price_24h"].get<std::string>());
            quote.lowestPrice24h_ = std::stod(dataObj["low_price_24h"].get<std::string>());
            quote.prevPrice1h_ = std::stod(dataObj["prev_price_1h"].get<std::string>());
            quote.price1hPcnt_ = std::stod(dataObj["price_1h_pcnt_e6"].get<std::string>()) / static_cast<double>(1e6);
            quote.markPrice_ = std::stod(dataObj["mark_price"].get<std::string>());
            quote.indexPrice_ = std::stod(dataObj["index_price"].get<std::string>());
            quote.openInterest_ = std::stod(dataObj["open_interest_e8"].get<std::string>()) / static_cast<double>(1e8);
            quote.totalTurnover_ = std::stod(dataObj["total_turnover_e8"].get<std::string>()) / static_cast<double>(1e8);
            quote.totalVolume_ = std::stod(dataObj["total_volume_e8"].get<std::string>()) / static_cast<double>(1e8);
            quote.turnover24h_ = std::stod(dataObj["turnover_24h_e8"].get<std::string>()) / static_cast<double>(1e8);
            quote.volume24h_ = std::stod(dataObj["volume_24h_e8"].get<std::string>()) / static_cast<double>(1e8);
            quote.fundingRate_ = std::stod(dataObj["funding_rate_e6"].get<std::string>()) / static_cast<double>(1e6);
            quote.predictFundingRate_ = std::stod(dataObj["predicted_funding_rate_e6"].get<std::string>()) / static_cast<double>(1e6);
            quote.fundingRateInterval_ = std::stod(dataObj["funding_rate_interval"].get<std::string>());
            quote.bestBidPrice_ = std::stod(dataObj["bid1_price"].get<std::string>());
            quote.bestAskPrice_ = std::stod(dataObj["ask1_price"].get<std::string>());
            quote.lastTickDirection_ = dataObj["last_tick_direction"].get<std::string>();
            quote.nextFundingTime_ = [&dataObj]() {
                auto dateTimeStr = dataObj["next_funding_time"].get<std::string>();
                return toTimestamp(dateTimeStr);
            }();
        } else {
            handleDelta(
                quote, [](auto &) {},
                [&dataObj = json["data"]["update"][0]](auto &quote) {
                    quote.lastPrice_ = dataObj.contains("last_price") ? std::stod(dataObj["last_price"].get<std::string>()) : quote.lastPrice_;
                    quote.prevPrice24h_ = dataObj.contains("prev_price_24h") ? std::stod(dataObj["prev_price_24h"].get<std::string>()) : quote.prevPrice24h_;
                    quote.price24hPcnt_ = dataObj.contains("price_24h_pcnt_e6") ? std::stod(dataObj["price_24h_pcnt_e6"].get<std::string>()) / static_cast<double>(1e6) : quote.price24hPcnt_;
                    quote.highestPrice24h_ = dataObj.contains("high_price_24h") ? std::stod(dataObj["high_price_24h"].get<std::string>()) : quote.highestPrice24h_;
                    quote.lowestPrice24h_ = dataObj.contains("low_price_24h") ? std::stod(dataObj["low_price_24h"].get<std::string>()) : quote.lowestPrice24h_;
                    quote.prevPrice1h_ = dataObj.contains("prev_price_1h") ? std::stod(dataObj["prev_price_1h"].get<std::string>()) : quote.prevPrice1h_;
                    quote.price1hPcnt_ = dataObj.contains("price_1h_pcnt_e6") ? std::stod(dataObj["price_1h_pcnt_e6"].get<std::string>()) / static_cast<double>(1e6) : quote.price1hPcnt_;
                    quote.markPrice_ = dataObj.contains("mark_price") ? std::stod(dataObj["mark_price"].get<std::string>()) : quote.markPrice_;
                    quote.indexPrice_ = dataObj.contains("index_price") ? std::stod(dataObj["index_price"].get<std::string>()) : quote.indexPrice_;
                    quote.openInterest_ = dataObj.contains("open_interest_e8") ? std::stod(dataObj["open_interest_e8"].get<std::string>()) / static_cast<double>(1e8) : quote.openInterest_;
                    quote.totalTurnover_ = dataObj.contains("total_turnover_e8") ? std::stod(dataObj["total_turnover_e8"].get<std::string>()) / static_cast<double>(1e8) : quote.totalTurnover_;
                    quote.totalVolume_ = dataObj.contains("total_volume_e8") ? std::stod(dataObj["total_volume_e8"].get<std::string>()) / static_cast<double>(1e8) : quote.totalVolume_;
                    quote.turnover24h_ = dataObj.contains("turnover_24h_e8") ? std::stod(dataObj["turnover_24h_e8"].get<std::string>()) / static_cast<double>(1e8) : quote.turnover24h_;
                    quote.volume24h_ = dataObj.contains("volume_24h_e8") ? std::stod(dataObj["volume_24h_e8"].get<std::string>()) / static_cast<double>(1e8) : quote.volume24h_;
                    quote.fundingRate_ = dataObj.contains("funding_rate_e6") ? std::stod(dataObj["funding_rate_e6"].get<std::string>()) / static_cast<double>(1e6) : quote.fundingRate_;
                    quote.predictFundingRate_ = dataObj.contains("predicted_funding_rate_e6") ? std::stod(dataObj["predicted_funding_rate_e6"].get<std::string>()) / static_cast<double>(1e6) : quote.predictFundingRate_;
                    quote.fundingRateInterval_ = dataObj.contains("funding_rate_interval") ? std::stod(dataObj["funding_rate_interval"].get<std::string>()) : quote.fundingRateInterval_;
                    quote.bestBidPrice_ = dataObj.contains("bid1_price") ? std::stod(dataObj["bid1_price"].get<std::string>()) : quote.bestBidPrice_;
                    quote.bestAskPrice_ = dataObj.contains("ask1_price") ? std::stod(dataObj["ask1_price"].get<std::string>()) : quote.bestAskPrice_;
                    quote.lastTickDirection_ = dataObj.contains("last_tick_direction") ? dataObj["last_tick_direction"].get<std::string>() : quote.lastTickDirection_;
                    quote.nextFundingTime_ = dataObj.contains("next_funding_time") ? [&dataObj]() {
                        auto dateTimeStr = dataObj["next_funding_time"].get<std::string>();
                        return toTimestamp(dateTimeStr);
                    }()
                                                                                   : quote.nextFundingTime_;
                },
                [](auto &) {});
        }
    }

    inline void updateMarketBook(MarketBook &quote, const nlohmann::json &json, bool isSnapshot)
    {
        if (isSnapshot) {
            quote.clear();
            const auto &orders = json["data"]["order_book"];
            for (const auto &order : orders) {
                const bool isBuyer = order["side"].get<std::string>() == "Buy";
                const auto price = std::stod(order["price"].get<std::string>());
                const auto qty = order["size"].get<double>();
                if (isBuyer) {
                    quote.pushBid(price, qty);
                } else {
                    quote.pushAsk(price, qty);
                }
            }
        } else {
            handleDelta(
                quote,
                [&json = json["data"]["delete"]](auto &quote) {
                    for (const auto &delObj : json) {
                        if (delObj["side"].get<std::string>() == "Buy") {
                            quote.deleteBid(stod(delObj["price"].get<std::string>()));
                        } else {
                            quote.deleteAsk(stod(delObj["price"].get<std::string>()));
                        }
                    }
                },
                [&json = json["data"]["update"]](auto &quote) {
                    for (const auto &updateObj : json) {
                        if (updateObj["side"].get<std::string>() == "Buy") {
                            quote.updateBid(stod(updateObj["price"].get<std::string>()), updateObj["size"].get<double>());
                        } else {
                            quote.updateAsk(stod(updateObj["price"].get<std::string>()), updateObj["size"].get<double>());
                        }
                    }
                },
                [&json = json["data"]["insert"]](auto &quote) {
                    for (const auto &insertObj : json) {
                        if (insertObj["side"].get<std::string>() == "Buy") {
                            quote.insertBid(stod(insertObj["price"].get<std::string>()), insertObj["size"].get<double>());
                        } else {
                            quote.insertAsk(stod(insertObj["price"].get<std::string>()), insertObj["size"].get<double>());
                        }
                    }
                });
        }
    }

    void updateKline(Kline &quote, const nlohmann::json &json)
    {
        const auto &klineArr = json["data"];
        if (klineArr.empty()) {
            return;
        }
        const auto &klineObj = klineArr[0];

        quote.type_ = klineObj["period"].get<std::string>();
        quote.highest_ = klineObj["high"].get<double>();
        quote.lowest_ = klineObj["low"].get<double>();
        quote.closed_ = klineObj["close"].get<double>();
        quote.opened_ = klineObj["open"].get<double>();
        quote.volume_ = std::stod(klineObj["volume"].get<std::string>());
        quote.turnover_ = std::stod(klineObj["turnover"].get<std::string>());
    }

    template <typename QuoteType, typename DeleteFunc, typename UpdateFunc, typename InsertFunc>
    void handleDelta(QuoteType &quote, DeleteFunc &&deleteFun, UpdateFunc &&updateFun, InsertFunc &&insertFun)
    {
        std::forward<DeleteFunc>(deleteFun)(quote);
        std::forward<UpdateFunc>(updateFun)(quote);
        std::forward<InsertFunc>(insertFun)(quote);
    }

    inline bool isPong(const nlohmann::json &msg)
    {
        auto iter = msg.find("ret_msg");
        return iter != msg.end() && *iter == "pong";
    }

    nlohmann::json config_;
    Util::Log::Logger logger_;
};

using BybitContractQuoteAdapter = QuoteAdapter<BybitContractQuoteHandler>;

} // namespace QuantCrypto::Quote

#endif // __BYBITCONTRACTQUOTE_H__