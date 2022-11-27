#ifndef __BINANCESPOTQUOTE_H__
#define __BINANCESPOTQUOTE_H__

#include "QuoteData.hpp"
#include "QuoteNode.hpp"
#include "QuoteAdapter.hpp"
#include "TimeUtils.hpp"
#include "StringUtil.hpp"

#include <cstddef>
#include <string>

#include "Logger.hpp"
#include <nlohmann/json.hpp>
#include <restclient-cpp/restclient.h>

using namespace Util::Time;
using namespace Util::StringUtil;

namespace QuantCrypto::Quote {

class BinanceSpotQuoteHandler : public QuoteNode
{
public:
    static inline const std::string Uri = "wss://stream.binance.com:443/ws";

    explicit BinanceSpotQuoteHandler(const nlohmann::json &config)
        : config_(config["exchange"]["binance"]["spot"])
        , logger_("BinanceSpotQuote")
    {
    }

    std::vector<nlohmann::json> genSubscribeMsg()
    {
        static const std::vector<std::string> Streams{"aggTrade", "depth@100ms", "kline"};

        std::vector<nlohmann::json> ret;

        const auto &symbols = config_["symbol"];
        static nlohmann::json req{
            {"method", "SUBSCRIBE"},
            {"id", 19940607}, // My birthday :)
        };

        auto params = nlohmann::json::array();
        for (const auto &stream : Streams) {
            if (stream == "kline") {
                const auto &klineTypes = config_["klineType"];
                for (const auto &klineType : klineTypes) {
                    for (const auto &symbol : symbols) {
                        std::string streamName = "";
                        std::string symbolStr = symbol.get<std::string>();
                        toLowerCase(symbolStr);
                        streamName += symbolStr;
                        streamName += "@";
                        streamName += "kline_";
                        streamName += klineType.get<std::string>();
                        params.emplace_back(streamName);
                    }
                }
                continue;
            }
            for (const auto &symbol : symbols) {
                std::string streamName = "";
                std::string symbolStr = symbol.get<std::string>();
                toLowerCase(symbolStr);
                streamName += symbolStr;
                streamName += "@";
                streamName += stream;
                params.emplace_back(streamName);
            }
        }
        req["params"] = params;
        ret.emplace_back(req);
        return ret;
    }

    void onMessage(const std::string &msg)
    {
        const auto jsonMsg = nlohmann::json::parse(msg);
        if (!jsonMsg.contains("e")) {
            return;
        }
        const auto &topic = jsonMsg["e"];
        const std::string &symbol = jsonMsg["s"].get<std::string>();
        if (topic == "aggTrade") {
            forQuoteData<Trade>(symbol, ExchangeT::Binance, jsonMsg, [this, symbol](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::Trade;
                updateHeader(quote, json, symbol);
                updateTrade(quote, json);
                logger_.debug("[Trade] {}", quote.dump());
            });
            return;
        }
        if (topic == "kline") {
            forQuoteData<Kline>(symbol, ExchangeT::Binance, jsonMsg, [this, symbol](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::Kline;
                updateHeader(quote, json, symbol);
                updateKline(quote, json);
                logger_.debug("[Kline] {}", quote.dump());
            });
            return;
        }
        if(topic == "depthUpdate") {
            if (!symbolBookInitInfoMap_.count(symbol)) {
                querySnapshotBook(symbol);
            }
            auto &updateByDiff = symbolBookInitInfoMap_[symbol].updateByDiffBook;
            if (!isDiffBookValid(jsonMsg, !updateByDiff)) {
                return;
            }
            updateByDiff = true;
            forQuoteData<MarketBook>(symbol, ExchangeT::Binance, jsonMsg, [this, symbol](const nlohmann::json &json, auto &quote) {
                quote.header_.type_ = QuoteType::MarketBook;
                updateHeader(quote, json, symbol);
                updateBook(quote, json);
                logger_.debug("[Book] {}", quote.dump());
            });
            return;
        }
    }

    static constexpr bool needPeriodicPing()
    {
        return false;
    }

    virtual bool enabled() override
    {
        return config_["enabled"].get<bool>();
    }

private:
    template <typename QuoteType>
    inline void updateHeader(QuoteType &quote, const nlohmann::json &json, const std::string &symbol)
    {
        quote.header_.source_ = ExchangeT::Binance;
        quote.header_.market_ = MarketT::Spot;
        quote.header_.symbol_ = symbol;
        quote.header_.receivedTime_ = getTime();
        if (json.contains("E")) { // snapshot book from api do not contains "E"
            quote.header_.sourceTime_ = json["E"].get<long long>();
        }
    }

    inline void updateTrade(Trade &quote, const nlohmann::json &json)
    {
        quote.tradeId_ = std::to_string(json["a"].get<long long>());  // Aggregate trade ID
        quote.tradeType_ = json["m"].get<bool>() ? TradeType::Buyer : TradeType::Seller;
        quote.price_ = std::stod(json["p"].get<std::string>());
        quote.qty_ = std::stod(json["q"].get<std::string>());
    }

    inline void updateSnapshotBook(MarketBook &quote, const nlohmann::json &json)
    {
        const auto &bidArr = json["bids"];
        for (const auto &bid : bidArr) {
            const auto price = std::stod(bid[0].get<std::string>());
            const auto qty = std::stod(bid[1].get<std::string>());
            quote.insertBid(price, qty);
        }

        const auto &askArr = json["asks"];
        for (const auto &ask : askArr) {
            const auto price = std::stod(ask[0].get<std::string>());
            const auto qty = std::stod(ask[1].get<std::string>());
            quote.insertAsk(price, qty);
        }
    }

    inline bool isDiffBookValid(const nlohmann::json &json, bool isFirstDiffBook)
    {
        const auto symbol = json["s"].get<std::string>();
        const auto lastUpdateId = symbolBookInitInfoMap_[symbol].lastUpdateId;
        if (isFirstDiffBook) {
            const auto firstUpdateId = json["U"].get<long long>();
            const auto finalUpdateId = json["u"].get<long long>();
            if (firstUpdateId > lastUpdateId ) {
                logger_.warn("invalid first diff book, firstUpdateId > lastUpdateId, symbol={}, lastUpdateId={}, firstUpdateId={}", symbol, lastUpdateId, firstUpdateId);
                return false;
            }
            if (finalUpdateId < lastUpdateId) {
                logger_.warn("invalid first diff book, finalUpdateId < lastUpdateId, symbol={}, lastUpdateId={}, finalUpdateId={}", symbol, lastUpdateId, finalUpdateId);
                return false;
            }
            return true;
        }
        // Drop any event where u(Final update ID in event) is < lastUpdateId in the snapshot
        const auto finalUpdateId = json["u"].get<long long>();
        if (finalUpdateId < lastUpdateId) {
            logger_.warn("invalid diff book, Final update < lastUpdateId, symbol={}, lastUpdateId={}, finalUpdateId={}", symbol, lastUpdateId, finalUpdateId);
            return false;
        }

        return true;
    }

    inline void updateBook(MarketBook &quote, const nlohmann::json &json)
    {
        const auto &bidArr = json["b"];
        for (const auto &bid : bidArr) {
            const auto price = std::stod(bid[0].get<std::string>());
            const auto qty = std::stod(bid[1].get<std::string>());
            if(qty == 0) {
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

    inline bool querySnapshotBook(const std::string &symbol, int limit = 100)
    {
        std::string url = fmt::format("https://api.binance.com/api/v3/depth?symbol={}&limit={}", symbol, limit);
        RestClient::Response r = RestClient::get(url);
        if (r.code != 200) {
            logger_.warn("querySnapshotBook failed, request={} r.code={}, r.body={}", url, r.code, r.body);
            return false;
        }
        const auto jsonMsg = nlohmann::json::parse(r.body);
        forQuoteData<MarketBook>(symbol, ExchangeT::Binance, jsonMsg, [this, symbol](const nlohmann::json &json, auto &quote) {
            quote.header_.type_ = QuoteType::MarketBook;
            updateHeader(quote, json, symbol);
            quote.clear();
            updateSnapshotBook(quote, json);
            symbolBookInitInfoMap_[symbol].lastUpdateId = json["lastUpdateId"].get<long long>();
            logger_.debug("[Book] {}", quote.dump());
        });
        return true;
    }

    inline void updateKline(Kline &quote, const nlohmann::json &json)
    {
        quote.type_ = json["k"]["i"].get<std::string>();    // Interval
        quote.highest_ = std::stod(json["k"]["h"].get<std::string>());
        quote.lowest_ = std::stod(json["k"]["l"].get<std::string>());
        quote.closed_ = std::stod(json["k"]["c"].get<std::string>());
        quote.opened_ = std::stod(json["k"]["o"].get<std::string>());
        quote.volume_ = std::stod(json["k"]["v"].get<std::string>());
        
        // Not support
        quote.turnover_ = -1.0;
    }

    nlohmann::json config_;
    Util::Log::Logger logger_;
    struct SymbolBookInitInfo
    {
        long long lastUpdateId = 0;
        bool updateByDiffBook = false;
    };
    std::unordered_map<std::string, SymbolBookInitInfo> symbolBookInitInfoMap_;
};

using BinanceSpotQuoteAdapter = QuoteAdapter<BinanceSpotQuoteHandler>;


} // namespace QuantCrypto::Quote
#endif // __BINANCESPOTQUOTE_H__