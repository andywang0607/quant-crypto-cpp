#ifndef __QUOTEDATA_H__
#define __QUOTEDATA_H__

#include <cstdint>
#include <string>
#include <typeinfo>

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

    Header(ExchangeT source, QuoteType type, const std::string &symbol)
        : source_(source)
        , type_(type)
        , symbol_(symbol)
    {
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

    double price_;
    double qty_;
};

struct MarketBook
{
    static constexpr size_t MaxDepth = 40;
    explicit MarketBook(const ExchangeT source, const std::string &symbol)
        : header_(source, QuoteType::MarketBook, symbol)
        , bidDepth_(0)
        , askDepth_(0)
    {
    }

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

    Info &ask(int index = 0)
    {
        return asks_[index];
    }

    Info &bid(int index = 0)
    {
        return bids_[index];
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
    explicit Trade(const ExchangeT source, const std::string &symbol)
        : header_(source, QuoteType::Trade, symbol)
    {
    }

    Header header_;

    std::string tradeId_;
    TradeType tradeType_;
    double price_;
    double qty_;
};

} // namespace QuantCrypto::Quote

#endif // __QUOTEDATA_H__