#ifndef __QUOTESERIALIZER_H__
#define __QUOTESERIALIZER_H__

#include "QuoteData.hpp"
#include <iomanip>
#include <ios>

namespace QuantCrypto::QuoteReceiver::QuoteUtil {

template <typename EnumType>
constexpr auto toUnderlying(EnumType &e)
{
    return static_cast<std::underlying_type_t<EnumType>>(e);
}

template <typename StreamType>
StreamType &operator<<(StreamType &s, const QuantCrypto::Quote::Header &header)
{
    s << header.sourceTime_ << ","
      << header.receivedTime_ << ","
      << toUnderlying(header.source_) << ","
      << toUnderlying(header.type_) << ","
      << header.symbol_;

    return s;
}

template <typename StreamType>
StreamType &operator<<(StreamType &s, const QuantCrypto::Quote::Info &info)
{
    s << std::setprecision(15)
      << std::noshowpoint
      << info.price_ << "@"
      << info.qty_;

    return s;
}

template <typename StreamType>
StreamType &operator<<(StreamType &s, const QuantCrypto::Quote::MarketBook &book)
{
    s << std::setprecision(15)
      << std::noshowpoint
      << book.header_ << ","
      << book.bidDepth_ << ","
      << book.askDepth_ << ",";

    for (int i = 0; i < book.bidDepth_; ++i) {
        s << book.bids_[i] << ",";
    }
    for (int i = 0; i < book.askDepth_; ++i) {
        s << book.asks_[i];
        if (i < book.askDepth_ - 1) {
            s << ",";
        }
    }

    return s;
}

template <typename StreamType>
StreamType &operator<<(StreamType &s, const QuantCrypto::Quote::Trade &trade)
{
    s << std::setprecision(15)
      << std::noshowpoint
      << trade.header_ << ","
      << trade.tradeId_ << ","
      << toUnderlying(trade.tradeType_) << ","
      << trade.price_ << "@" << trade.qty_;

    return s;
}

} // namespace QuantCrypto::QuoteReceiver::Util

#endif // __QUOTESERIALIZER_H__