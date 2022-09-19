#ifndef __BYBITQUOTE_H__
#define __BYBITQUOTE_H__

#include "QuoteData.hpp"
#include "QuoteNode.hpp"
#include "QuoteAdapter.hpp"
#include "TimeUtils.hpp"

#include <cstddef>
#include <string>

#include <nlohmann/json.hpp>
#include "Logger.hpp"

using namespace Util::Time;

namespace QuantCrypto::Quote {

class BybitSpotQuoteHandler : public QuoteNode
{
public:
    static inline const std::string Uri = "wss://stream.bybit.com/spot/quote/ws/v2";

    explicit BybitSpotQuoteHandler(const nlohmann::json &config)
        : config_(config["exchange"]["bybit"]["spot"])
        , logger_("BybitSpotQuote")
    {
    }

    std::vector<nlohmann::json> genSubscribeMsg()
    {
        std::vector<nlohmann::json> ret;

        const auto &symbols = config_["symbol"];

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

        const auto &klineType = config_["klineType"];
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
        if (jsonMsg.contains("pong")) {
            logger_.info(jsonMsg.dump());
            return;
        }
        if (!jsonMsg.contains("data")) {
            return;
        }
        const auto &topic = jsonMsg["topic"];

        const std::string &symbol = jsonMsg["params"]["symbol"].get<std::string>();
        const std::string exchangeSymbol = symbol + ".Bybit";

        if (topic == "trade") {
            forQuoteData<Trade>(exchangeSymbol, jsonMsg, [this, exchangeSymbol](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::Trade;
                updateHeader(quote, json, exchangeSymbol);
                updateTrade(quote, json);
                logger_.debug("[Trade] {}", quote.dump());
            });
            return;
        }

        if (topic == "depth") {
            forQuoteData<MarketBook>(exchangeSymbol, jsonMsg, [this, exchangeSymbol](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::MarketBook;
                updateHeader(quote, json, exchangeSymbol);
                quote.clear();
                updateBook(quote, json);
                logger_.debug("[Book] {}", quote.dump());
            });
            return;
        }

        if (topic == "kline") {
            forQuoteData<Kline>(exchangeSymbol, jsonMsg, [this, exchangeSymbol](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::Kline;
                updateHeader(quote, json, exchangeSymbol);
                updateKline(quote, json);
                logger_.debug("[Kline] {}", quote.dump());
            });
            return;
        }
    }

    nlohmann::json genPingMessage()
    {
        return {
            {"ping", 1535975085152}};
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
        quote.header_.symbol_ = symbol;
        quote.header_.receivedTime_ = getTime();
        quote.header_.sourceTime_ = json["data"].at("t").get<long long>();
    }

    inline void updateTrade(Trade &quote, const nlohmann::json &json)
    {
        quote.tradeId_ = json["data"]["v"].get<std::string>();
        quote.tradeType_ = json["data"]["m"].get<bool>() ? TradeType::Buyer : TradeType::Seller;
        quote.price_ = std::stod(json["data"]["p"].get<std::string>());
        quote.qty_ = std::stod(json["data"]["q"].get<std::string>());
    }

    inline void updateBook(MarketBook &quote, const nlohmann::json &json)
    {
        const auto &bidArr = json["data"]["b"];
        for (const auto &bid : bidArr) {
            quote.pushBid(std::stod(bid[0].get<std::string>()), std::stod(bid[1].get<std::string>()));
        }

        const auto &askArr = json["data"]["a"];
        for (const auto &ask : askArr) {
            quote.pushAsk(std::stod(ask[0].get<std::string>()), std::stod(ask[1].get<std::string>()));
        }
    }

    inline void updateKline(Kline &quote, const nlohmann::json &json)
    {
        quote.type_ = json["params"]["klineType"].get<std::string>();
        quote.highest_ = std::stod(json["data"]["h"].get<std::string>());
        quote.lowest_ = std::stod(json["data"]["l"].get<std::string>());
        quote.closed_ = std::stod(json["data"]["c"].get<std::string>());
        quote.opened_ = std::stod(json["data"]["o"].get<std::string>());
        quote.volume_ = std::stod(json["data"]["v"].get<std::string>());
    }

    nlohmann::json config_;
    Util::Log::Logger logger_;
};

using BybitSpotQuoteAdapter = QuoteAdapter<BybitSpotQuoteHandler>;


} // namespace QuantCrypto::Quote
#endif // __BYBITQUOTE_H__