#ifndef __QUOTEDATA_H__
#define __QUOTEDATA_H__

#include <cstdint>
#include <string>
#include <typeinfo>

#include <spdlog/fmt/fmt.h>

namespace QuantCrypto::Quote {

enum class QuoteType : char
{
    MarketBook = '1',
    Trade = '2'
};

enum class ExchangeT : int8_t
{
    ByBit = 1,
};

struct Header
{
    ExchangeT source_;
    QuoteType type_;
    std::string symbol_;
    long long sourceTime_;
    long long receivedTime_;

    Header() = default;
    ~Header() = default;

    inline std::string dump()
    {
        return fmt::format("source={}, type={}, symbol={}, sourceTime={}, receivedTime={}",
                           source_, type_, symbol_, sourceTime_, receivedTime_);
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

    inline std::string dump()
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

    inline std::string dump()
    {
        return fmt::format("header={}, bidDepth={}, askDepth={}, bid={}, {}, {}, {}, {}, ask={}, {}, {}, {}, {}",
                           header_.dump(), bidDepth_, askDepth_, 
                           bids_[0].dump(), bids_[1].dump(), bids_[2].dump(), bids_[3].dump(), bids_[4].dump(),
                           asks_[0].dump(), asks_[1].dump(), asks_[2].dump(), asks_[3].dump(), asks_[4].dump());
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

    inline std::string dump()
    {
        return fmt::format("header={}, tradeId_={}, tradeType_={}, price_={}, qty_={}",
                           header_.dump(), tradeId_, tradeType_, price_, qty_);
    }
};

} // namespace QuantCrypto::Quote

#endif // __QUOTEDATA_H__