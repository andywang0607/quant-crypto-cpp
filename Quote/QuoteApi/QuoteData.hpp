#ifndef __QUOTEDATA_H__
#define __QUOTEDATA_H__

#include <cstdint>
#include <string>
#include <typeinfo>

#include <spdlog/fmt/fmt.h>

namespace QuantCrypto::Quote {

enum class QuoteType : char
{
    MarketBook = 'M',
    Trade = 'T',
    Kline = 'K',
    InstrumentInfo = 'I',
};

enum class ExchangeT : char
{
    ByBit = '1',
    Binance = '2',
};

enum class MarketT : char
{
    Spot = '1',
    USDTPerpetual = '2',
};

struct Header
{
    ExchangeT source_;
    MarketT market_;
    QuoteType type_;
    std::string symbol_;
    long long sourceTime_;
    long long receivedTime_;

    Header() = default;
    ~Header() = default;

    inline std::string dump() const
    {
        return fmt::format("source={}, market={}, type={}, symbol={}, sourceTime={}, receivedTime={}",
                           source_, market_, type_, symbol_, sourceTime_, receivedTime_);
    }
};

struct Info
{
    Info() = default;
    Info(double price, double qty)
        : price_(price)
        , qty_(qty)
    {
    }

    inline std::string dump() const
    {
        return fmt::format("({}, {})",
                           price_, qty_);
    }

    double price_;
    double qty_;
};

struct MarketBook
{
    static constexpr size_t MaxDepth = 40;

    MarketBook() = default;
    ~MarketBook() = default;

    void pushBid(const double price, const double qty)
    {
        if (bidDepth_ >= MaxDepth) {
            return;
        }

        auto &bid = bids_[bidDepth_];
        bid.price_ = price;
        bid.qty_ = qty;
        bidDepth_++;
    }

    void pushAsk(const double price, const double qty)
    {
        if (askDepth_ >= MaxDepth) {
            return;
        }

        auto &ask = asks_[askDepth_];
        ask.price_ = price;
        ask.qty_ = qty;
        askDepth_++;
    }

    void insertBid(const double price, const double qty)
    {
        if (bidDepth_ >= MaxDepth) {
            return;
        }
        for (int i = 0; i < bidDepth_; ++i) {
            if (price > bids_[i].price_) {
                continue;
            }
            memcpy(&bids_[i] + 1, &bids_[i], (bidDepth_ - i) * sizeof(Info));
            bids_[i].price_ = price;
            bids_[i].qty_ = qty;
            ++bidDepth_;
            return;
        }
        bids_[bidDepth_].price_ = price;
        bids_[bidDepth_].qty_ = qty;
        ++bidDepth_;
    }

    void insertAsk(const double price, const double qty)
    {
        if (askDepth_ >= MaxDepth) {
            return;
        }
        for (int i = 0; i < askDepth_; ++i) {
            if (price > asks_[i].price_) {
                continue;
            }
            memcpy(&asks_[i] + 1, &asks_[i], (askDepth_ - i) * sizeof(Info));
            asks_[i].price_ = price;
            asks_[i].qty_ = qty;
            ++askDepth_;
            return;
        }
        asks_[askDepth_].price_ = price;
        asks_[askDepth_].qty_ = qty;
        ++askDepth_;
    }

    bool updateBid(const double price, const double qty)
    {
        for (int i = 0; i < bidDepth_; ++i) {
            if (bids_[i].price_ == price) {
                bids_[i].qty_ = qty;
                return true;
            }
        }
        return false;
    }

    bool updateAsk(const double price, const double qty)
    {
        for (int i = 0; i < askDepth_; ++i) {
            if (asks_[i].price_ == price) {
                asks_[i].qty_ = qty;
                return true;
            }
        }
        return false;
    }

    void deleteBid(const double price)
    {
        for (int i = 0; i < bidDepth_; ++i) {
            if (bids_[i].price_ == price) {
                memmove((void *)&bids_[i], (void *)&bids_[i + 1], (bidDepth_ - i - 1) * sizeof(Info));
                --bidDepth_;
                break;
            }
        }
    }

    void deleteAsk(const double price)
    {
        for (int i = 0; i < askDepth_; ++i) {
            if (asks_[i].price_ == price) {
                memmove((void *)&asks_[i], (void *)&asks_[i + 1], (askDepth_ - i - 1) * sizeof(Info));
                --askDepth_;
                break;
            }
        }
    }

    void clear()
    {
        const int depth = bidDepth_ > askDepth_ ? bidDepth_ : askDepth_;
        for (int i = 0; i < depth; ++i) {
            bids_[i] = Info(0, 0);
            asks_[i] = Info(0, 0);
        }

        bidDepth_ = 0;
        askDepth_ = 0;
    }

    Info &ask(int index = 0)
    {
        return asks_[index];
    }

    Info &bid(int index = 0)
    {
        return bids_[index];
    }

    inline std::string dump() const
    {
        return fmt::format("header={}, bidDepth={}, askDepth={}, bid={}, {}, {}, {}, {}, {}, {}, {}, {}, {}, ask={}, {}, {}, {}, {}, {}, {}, {}, {}, {}",
                           header_.dump(), bidDepth_, askDepth_,
                           bids_[0].dump(), bids_[1].dump(), bids_[2].dump(), bids_[3].dump(), bids_[4].dump(), bids_[5].dump(), bids_[6].dump(), bids_[7].dump(), bids_[8].dump(), bids_[9].dump(),
                           asks_[0].dump(), asks_[1].dump(), asks_[2].dump(), asks_[3].dump(), asks_[4].dump(), asks_[5].dump(), asks_[6].dump(), asks_[7].dump(), asks_[8].dump(), asks_[9].dump());
    }

    Header header_;

    int bidDepth_;
    int askDepth_;
    Info bids_[MaxDepth];
    Info asks_[MaxDepth];
};

enum class TradeType : char
{
    Buyer = 'B',
    Seller = 'S',
};

struct Trade
{
    Trade() = default;
    ~Trade() = default;

    Header header_;

    std::string tradeId_;
    TradeType tradeType_;
    double price_;
    double qty_;

    inline std::string dump() const
    {
        return fmt::format("header={}, tradeId_={}, tradeType_={}, price_={}, qty_={}",
                           header_.dump(), tradeId_, tradeType_, price_, qty_);
    }
};

struct Kline
{
    Kline() = default;
    ~Kline() = default;

    Header header_;

    std::string type_;

    double highest_;
    double lowest_;
    double closed_;
    double opened_;
    double volume_;
    double turnover_;

    inline std::string dump() const
    {
        return fmt::format("header={}, type={}, highest={}, lowest={}, closed={}, opened={}, volume={} turnover={}",
                           header_.dump(), type_, highest_, lowest_, closed_, opened_, volume_, turnover_);
    }
};

struct InstrumentInfo
{
    InstrumentInfo() = default;
    ~InstrumentInfo() = default;

    Header header_;

    double lastPrice_;
    double prevPrice24h_;
    double price24hPcnt_;
    double highestPrice24h_;
    double lowestPrice24h_;

    double prevPrice1h_;
    double price1hPcnt_;
    double markPrice_;

    double indexPrice_;
    double openInterest_;
    double totalTurnover_;
    double totalVolume_;

    double turnover24h_;
    double volume24h_;

    double fundingRate_;
    double predictFundingRate_;

    long long nextFundingTime_;
    int fundingRateInterval_;

    double bestBidPrice_;
    double bestAskPrice_;

    std::string lastTickDirection_;

    inline std::string dump() const
    {
        return fmt::format(
            "header={},"
            "lastPrice_={}, prevPrice24h_={}, price24hPcnt_={}, highestPrice24h_={}, lowestPrice24h_={},"
            "prevPrice1h_={}, price1hPcnt_={},"
            "markPrice_ = {}, indexPrice_ = {},"
            "openInterest_ = {}, totalTurnover_ = {}, totalVolume_ = {},"
            "turnover24h_ = {}, volume24h_ = {},"
            "fundingRate_ = {}, predictFundingRate_ = {}, nextFundingTime_ = {}, fundingRateInterval_ = {},"
            "bestBidPrice_ = {}, bestAskPrice_ = {}, lastTickDirection_ = {}",
            header_.dump(),
            lastPrice_, prevPrice24h_, price24hPcnt_, highestPrice24h_, lowestPrice24h_,
            prevPrice1h_, price1hPcnt_,
            markPrice_, indexPrice_,
            openInterest_, totalTurnover_, totalVolume_,
            turnover24h_, volume24h_,
            fundingRate_, predictFundingRate_, nextFundingTime_, fundingRateInterval_,
            bestBidPrice_, bestAskPrice_, lastTickDirection_);
    }
};

} // namespace QuantCrypto::Quote

#endif // __QUOTEDATA_H__