#include <gtest/gtest.h>
#include <sstream>

#include "QuoteSerializer.hpp"

using namespace QuantCrypto::QuoteReceiver::QuoteUtil;

using namespace QuantCrypto::Quote;

struct MockQuote
{
    static Header genHeader(QuoteType type)
    {
        Header header;
        header.source_ = ExchangeT::ByBit;
        header.type_ = type;
        header.symbol_ = "ETHUSDT";
        header.sourceTime_ = 1111111111LL;
        header.receivedTime_ = 22222222222LL;

        return header;
    }

    static Info genInfo()
    {
        return Info(2000.12345, 100.123456789);
    }

    static MarketBook genBook()
    {
        MarketBook book;
        book.header_ = genHeader(QuoteType::MarketBook);
        book.bidDepth_ = 5;
        book.askDepth_ = 4;
        for (int i = 0; i < book.bidDepth_; ++i) {
            book.bids_[i] = Info(2000.22, 100);
        }
        for (int i = 0; i < book.askDepth_; ++i) {
            book.asks_[i] = Info(2000.1, 200);
        }
        return book;
    }

    static Trade genTrade()
    {
        Trade trade;
        trade.header_ = genHeader(QuoteType::Trade);
        trade.tradeId_ = "tradeId";
        trade.tradeType_ = TradeType::Buyer;
        trade.price_ = 2000.123456;
        trade.qty_ = 1000000000.123;

        return trade;
    }
};

TEST(QuoteSerializerTest, Header)
{
    const auto header = MockQuote::genHeader(QuoteType::MarketBook);
    std::stringstream ss;
    ss << header;

    const auto golden = "1111111111,22222222222,1,M,ETHUSDT";

    EXPECT_EQ(ss.str(), golden);
};

TEST(QuoteSerializerTest, Info)
{
    const auto info = MockQuote::genInfo();
    std::stringstream ss;
    ss << info;

    const auto golden = "2000.12345@100.123456789";

    EXPECT_EQ(ss.str(), golden);
};

TEST(QuoteSerializerTest, MarketBook)
{
    const auto book = MockQuote::genBook();
    std::stringstream ss;
    ss << book;

    const auto golden = "1111111111,22222222222,1,M,ETHUSDT,5,4,2000.22@100,2000.22@100,2000.22@100,2000.22@100,2000.22@100,2000.1@200,2000.1@200,2000.1@200,2000.1@200";

    EXPECT_EQ(ss.str(), golden);
};

TEST(QuoteSerializerTest, Trade)
{
    const auto trade = MockQuote::genTrade();
    std::stringstream ss;
    ss << trade;

    const auto golden = "1111111111,22222222222,1,T,ETHUSDT,tradeId,B,2000.123456@1000000000.123";

    EXPECT_EQ(ss.str(), golden);
};