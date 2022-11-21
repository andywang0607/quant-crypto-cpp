#ifndef __QUOTEWRITTER_H__
#define __QUOTEWRITTER_H__

#include "QuoteApi.hpp"
#include "QuoteData.hpp"
#include "QuoteSerializer.hpp"
#include "TimeUtils.hpp"
#include "Logger.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace QuantCrypto::QuoteReceiver::QuoteUtil {

struct FileWriter
{
    template <typename... Args>
    FileWriter(Args &&... args)
        : ofstream_(std::forward<Args>(args)...)
    {
    }

    template <typename DataType>
    std::ofstream &operator<<(DataType &data)
    {
        std::lock_guard lk(mtx_);
        ofstream_ << data;
        return ofstream_;
    }

    void flush()
    {
        ofstream_.flush();
    }

    std::mutex mtx_;
    std::ofstream ofstream_;
};

template <typename QuoteType>
class QuoteWritter
{
public:
    QuoteWritter(const nlohmann::json &config)
        : root_(config["QuoteReceiver"]["root"].get<std::string>())
        , logger_("QuoteWritter")
    {
    }

    void init(const std::string &symbol, const QuoteType &quote)
    {
        const std::string date = Util::Time::getDate();
        const std::string exchange = getExchange(quote);
        const auto symbolWithExchange = symbol + "." + exchange;

        const std::string path = [&]() {
            if (root_.empty()) {
                return getTypeFodlerName(quote) + "/" + exchange + "/" + symbolWithExchange + "/" + date + ".txt";
            }
            return root_ + "/" + getTypeFodlerName(quote) + "/" + symbolWithExchange + "/" + date + ".txt";
        }();

        try {
            std::string dirName = getDirname(path);
            std::filesystem::create_directories(dirName);
            mapLock_.lock();
            auto &symbolWriterMap = fileWriterMap_[quote.header_.source_];
            auto iter = symbolWriterMap.try_emplace(symbol, path, std::ios_base::app);
            if (iter.second) {
                logger_.info("New file: {}", path);
            }
            mapLock_.unlock();
            auto &symbol = iter.first->first;
            auto &fileWriter = iter.first->second;

            if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::MarketBook>) {
                QuantCrypto::Quote::QuoteApi::onNewBook.subscribe([&symbol, &fileWriter, initExchange = quote.header_.source_, initMarket = quote.header_.market_](auto &exchange, auto &book) {
                    const auto receivedSymbol = book.header_.symbol_;
                    if(exchange != initExchange) {
                        return;
                    }
                    if (book.header_.market_ != initMarket) {
                        return;
                    }
                    if (receivedSymbol != symbol) {
                        return;
                    }
                    fileWriter << book << "\n";
                });
            }
            if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::Trade>) {
                QuantCrypto::Quote::QuoteApi::onNewTrade.subscribe([&symbol, &fileWriter, initExchange = quote.header_.source_, initMarket = quote.header_.market_](auto &exchange, auto &trade) {
                    static int count = 0;
                    const auto receivedSymbol = trade.header_.symbol_;
                    if(exchange != initExchange) {
                        return;
                    }
                    if (trade.header_.market_ != initMarket) {
                        return;
                    }
                    if (receivedSymbol != symbol) {
                        return;
                    }
                    fileWriter << trade << "\n";
                    if (count++ >= 5) {
                        fileWriter.flush();
                        count = 0;
                    }
                });
            }
            if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::Kline>) {
                QuantCrypto::Quote::QuoteApi::onNewKline.subscribe([&symbol, &fileWriter, initExchange = quote.header_.source_, initMarket = quote.header_.market_](auto &exchange, auto &kline) {
                    static int count = 0;
                    const auto receivedSymbol = kline.header_.symbol_;
                    if(exchange != initExchange) {
                        return;
                    }
                    if (kline.header_.market_ != initMarket) {
                        return;
                    }
                    if (receivedSymbol != symbol) {
                        return;
                    }
                    fileWriter << kline << "\n";
                    if (count++ >= 5) {
                        fileWriter.flush();
                        count = 0;
                    }
                });
            }
        } catch (std::exception &e) {
            logger_.error("exception: {}", e.what());
        }
    }

private:
    constexpr std::string getTypeFodlerName(const QuoteType &quote)
    {
        if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::MarketBook>) {
            return "asciidata_book";
        }
        if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::Trade>) {
            return "asciidata_trade";
        }
        if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::Kline>) {
            return "asciidata_kline";
        }
    }

    std::string getDirname(const std::string &path)
    {
        return std::filesystem::path(path).parent_path();
    }

    std::string getExchange(const QuoteType &quote)
    {
        if (quote.header_.source_ == QuantCrypto::Quote::ExchangeT::ByBit) {
            return "Bybit";
        }
        if (quote.header_.source_ == QuantCrypto::Quote::ExchangeT::Binance) {
            return "Binance";
        }
        return "";
    }

    std::string root_;
    std::mutex mapLock_;
    using SymbolWriterMap = std::unordered_map<std::string, FileWriter>;
    std::unordered_map<QuantCrypto::Quote::ExchangeT, SymbolWriterMap> fileWriterMap_;
    Util::Log::Logger logger_;
};
} // namespace QuantCrypto::QuoteReceiver::QuoteUtil

#endif // __QUOTEWRITTER_H__