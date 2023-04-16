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

namespace QuantCrypto::Quote {

class BybitContractQuoteHandler : public QuoteNode
{
public:
    static inline const std::string Uri = "wss://stream.bybit.com/v5/public/linear";

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
            static nlohmann::json req;

            req["op"] = "subscribe";
            req["args"] = nlohmann::json::array(
                {fmt::format("publicTrade.{}", symbol),
                 fmt::format("orderbook.50.{}", symbol)});

            ret.emplace_back(req);
        }

        const auto &klineType = config_["klineType"];
        for (const auto &symbol : symbols) {
            for (const auto &type : klineType) {
                static nlohmann::json klineReq;

                klineReq["op"] = "subscribe";
                klineReq["args"] = nlohmann::json::array({fmt::format("kline.{}.{}", type, symbol)});

                ret.emplace_back(klineReq);
            }
        }

        return ret;
    }

    void onMessage(const std::string &msg)
    {
        const auto jsonMsg = nlohmann::json::parse(msg);
        if (jsonMsg.contains("pong")) {
            logger_.info(jsonMsg.dump());
            return;
        }
        if (!jsonMsg.contains("data")) {
            return;
        }
        const auto &topic = jsonMsg["topic"].get<std::string>();

        if (topic.find("publicTrade") != std::string::npos) {
            const auto &dataObj = jsonMsg["data"][0];
            const auto &symbol = dataObj["s"].get<std::string>();
            forQuoteData<Trade>(symbol, ExchangeT::ByBit, jsonMsg, [this, &symbol, &dataObj](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::Trade;
                updateHeader(quote, json, symbol);
                updateTrade(quote, dataObj);
                logger_.debug("[Trade] {}", quote.dump());
            });
            return;
        }

        if (topic.find("orderbook") != std::string::npos) {
            const auto &dataObj = jsonMsg["data"];
            const auto symbol = [&topic]() -> std::string {
                auto found = topic.find_last_of(".");
                return topic.substr(found + 1);
            }();
            const auto isSnapshot = jsonMsg["type"].get<std::string>() == "snapshot";
            forQuoteData<MarketBook>(symbol, ExchangeT::ByBit, jsonMsg, [this, &symbol, &dataObj, isSnapshot](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::MarketBook;
                updateHeader(quote, json, symbol);
                updateBook(quote, dataObj, isSnapshot);
                logger_.info("[Book] {}", quote.dump());
            });
            return;
        }

        if (topic.find("kline") != std::string::npos) {
            const auto &dataObjArr = jsonMsg["data"];
            const auto symbol = [&topic]() -> std::string {
                auto found = topic.find_last_of(".");
                return topic.substr(found + 1);
            }();
            const auto &dataObj = jsonMsg["data"][0];
            forQuoteData<Kline>(symbol, ExchangeT::ByBit, jsonMsg, [this, &symbol, &dataObj](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::Kline;
                updateHeader(quote, json, symbol);
                updateKline(quote, dataObj);
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
    template <typename QuoteType>
    inline void updateHeader(QuoteType &quote, const nlohmann::json &json, const std::string &symbol)
    {
        quote.header_.source_ = ExchangeT::ByBit;
        quote.header_.market_ = MarketT::USDTPerpetual;
        quote.header_.symbol_ = symbol;
        quote.header_.receivedTime_ = Util::Time::getTime();
        quote.header_.sourceTime_ = json["ts"].get<long long>();
    }

    inline void updateTrade(Trade &quote, const nlohmann::json &json)
    {
        quote.tradeId_ = json["i"].get<std::string>();
        quote.tradeType_ = json["S"].get<std::string>() == "Buy" ? TradeType::Buyer : TradeType::Seller;
        quote.price_ = std::stod(json["p"].get<std::string>());
        quote.qty_ = std::stod(json["v"].get<std::string>());
    }

    inline void updateBook(MarketBook &quote, const nlohmann::json &json, bool isSnapshot)
    {
        if (isSnapshot) {
            updateSnapshotBook(quote, json);
        } else {
            updateDeltaBook(quote, json);
        }
    }

    inline void updateSnapshotBook(MarketBook &quote, const nlohmann::json &json)
    {
        quote.clear();
        const auto &bidArr = json["b"];
        for (const auto &order : bidArr) {
            const auto price = std::stod(order[0].get<std::string>());
            const auto qty = std::stod(order[1].get<std::string>());
            
            // Bybit's snapshot book guaranty bids array is in descending order
            quote.pushBid(price, qty);
        }

        const auto &askArr = json["a"];
        for (const auto &order : askArr) {
            const auto price = std::stod(order[0].get<std::string>());
            const auto qty = std::stod(order[1].get<std::string>());

            // Bybit's snapshot book guaranty bids array is in asscending order
            quote.pushAsk(price, qty);
        }
    }

    inline void updateDeltaBook(MarketBook &quote, const nlohmann::json &json)
    {
        const auto &bidArr = json["b"];
        for (const auto &bid : bidArr) {
            const auto price = std::stod(bid[0].get<std::string>());
            const auto qty = std::stod(bid[1].get<std::string>());
            if (qty == 0) {
                quote.deleteBid(price);
                continue;
            }
            quote.insertBid(price, qty);
        }

        const auto &askArr = json["a"];
        for (const auto &ask : askArr) {
            const auto price = std::stod(ask[0].get<std::string>());
            const auto qty = std::stod(ask[1].get<std::string>());
            if (qty == 0) {
                quote.deleteAsk(price);
                continue;
            }
            quote.insertAsk(price, qty);
        }
    }

    inline void updateKline(Kline &quote, const nlohmann::json &json)
    {
        quote.type_ = json["interval"].get<std::string>();
        quote.highest_ = std::stod(json["high"].get<std::string>());
        quote.lowest_ = std::stod(json["low"].get<std::string>());
        quote.closed_ = std::stod(json["close"].get<std::string>());
        quote.opened_ = std::stod(json["open"].get<std::string>());
        quote.volume_ = std::stod(json["volume"].get<std::string>());
        quote.turnover_ = std::stod(json["turnover"].get<std::string>());
    }

    nlohmann::json config_;
    Util::Log::Logger logger_;
};

using BybitContractQuoteAdapter = QuoteAdapter<BybitContractQuoteHandler>;

} // namespace QuantCrypto::Quote

#endif // __BYBITCONTRACTQUOTE_H__