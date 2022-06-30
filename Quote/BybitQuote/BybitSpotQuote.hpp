#ifndef __BYBITQUOTE_H__
#define __BYBITQUOTE_H__

#include "QuoteData.hpp"
#include "QuoteNode.hpp"
#include "WebSocketReceiver.hpp"

#include <cstddef>
#include <string>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace QuantCrypto::Quote {

class BybitSpotQuoteHandler : public QuoteNode
{
public:
    static inline const std::string Uri = "wss://stream.bybit.com/spot/quote/ws/v2";

    explicit BybitSpotQuoteHandler(const nlohmann::json &config)
        : config_(config)
    {
    }

    std::vector<nlohmann::json> genSubscribeMsg()
    {
        std::vector<nlohmann::json> ret;

        const auto &symbols = config_["exchange"]["bybit"]["symbol"];

        for (const auto &symbol : symbols) {
            static nlohmann::json tradeReq;

            tradeReq["topic"] = "trade";
            tradeReq["event"] = "sub";
            tradeReq["params"] = {{"binary", false}, {"symbol", symbol}};

            ret.emplace_back(tradeReq);
        }

        for (const auto &symbol : symbols) {
            static nlohmann::json bookReq;

            bookReq["topic"] = "depth";
            bookReq["event"] = "sub";
            bookReq["params"] = {{"binary", false}, {"symbol", symbol}};

            ret.emplace_back(bookReq);
        }

        const auto &klineType = config_["exchange"]["bybit"]["klineType"];
        for (const auto &symbol : symbols) {
            for (const auto &type : klineType) {
                static nlohmann::json klineReq;

                klineReq["topic"] = "kline";
                klineReq["event"] = "sub";
                klineReq["params"] = {{"binary", false}, {"symbol", symbol}, {"klineType", type}};

                ret.emplace_back(klineReq);
            }
        }

        return ret;
    }

    void onMessage(const std::string &msg)
    {
        const auto jsonMsg = nlohmann::json::parse(msg);
        const auto &topic = jsonMsg["topic"];

        if (topic == "trade") {
            handleTrade(jsonMsg);
            return;
        }

        if (topic == "depth") {
            handleBook(jsonMsg);
            return;
        }

        if (topic == "kline") {
            handleKline(jsonMsg);
            return;
        }
    }

private:
    template <typename T>
    inline T &handleHeader(const nlohmann::json &json)
    {
        const std::string &symbol = json["params"]["symbol"].get<std::string>();
        T &quote = [this, &symbol]() -> T & {
            if constexpr (std::is_same_v<T, Trade>) {
                return trade_[symbol];
            }
            if constexpr (std::is_same_v<T, MarketBook>) {
                return marketBook_[symbol];
            }
            if constexpr (std::is_same_v<T, Kline>) {
                return kline_[symbol];
            }
        }();

        quote.header_.source_ = ExchangeT::ByBit;
        quote.header_.type_ = []() {
            if constexpr (std::is_same_v<T, Trade>) {
                return QuoteType::Trade;
            }
            if constexpr (std::is_same_v<T, MarketBook>) {
                return QuoteType::MarketBook;
            }
            if constexpr (std::is_same_v<T, Kline>) {
                return QuoteType::Kline;
            }
        }();
        quote.header_.symbol_ = symbol;
        quote.header_.receivedTime_ = getTime();
        quote.header_.sourceTime_ = json["data"].at("t").get<long long>();

        return quote;
    }

    inline void handleTrade(const nlohmann::json &json)
    {
        if (!json.contains("data")) {
            return;
        }

        auto &trade = handleHeader<Trade>(json);

        trade.tradeId_ = json["data"]["v"].get<std::string>();
        trade.tradeType_ = json["data"]["m"].get<bool>() ? TradeType::Buyer : TradeType::Seller;
        trade.price_ = std::stod(json["data"]["p"].get<std::string>());
        trade.qty_ = std::stod(json["data"]["q"].get<std::string>());

        spdlog::info("[Trade] {}", trade.dump());
    }

    inline void handleBook(const nlohmann::json &json)
    {
        if (!json.contains("data")) {
            return;
        }

        auto &book = handleHeader<MarketBook>(json);
        book.clear();

        const auto &bidArr = json["data"]["b"];
        for (const auto &bid : bidArr) {
            book.pushBid(std::stod(bid[0].get<std::string>()), std::stod(bid[1].get<std::string>()));
        }

        const auto &askArr = json["data"]["a"];
        for (const auto &ask : askArr) {
            book.pushAsk(std::stod(ask[0].get<std::string>()), std::stod(ask[1].get<std::string>()));
        }

        spdlog::info("[Book] {}", book.dump());
    }

    inline void handleKline(const nlohmann::json &json)
    {
        if (!json.contains("data")) {
            return;
        }

        auto &kline = handleHeader<Kline>(json);

        kline.type_ = json["params"]["klineType"].get<std::string>();
        kline.highest_ = std::stod(json["data"]["h"].get<std::string>());
        kline.lowest_ = std::stod(json["data"]["l"].get<std::string>());
        kline.closed_ = std::stod(json["data"]["c"].get<std::string>());
        kline.opened_ = std::stod(json["data"]["o"].get<std::string>());
        kline.volume_ = std::stod(json["data"]["v"].get<std::string>());

        spdlog::info("[Kline] {}", kline.dump());
    }

    nlohmann::json config_;
};

using BybitSpotQuoteAdapter = WebSocketReceiver<BybitSpotQuoteHandler>;


} // namespace QuantCrypto::Quote
#endif // __BYBITQUOTE_H__