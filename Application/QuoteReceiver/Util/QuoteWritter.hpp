#ifndef __QUOTEWRITTER_H__
#define __QUOTEWRITTER_H__

#include "Logger.hpp"
#include "QuoteApi.hpp"
#include "QuoteData.hpp"
#include "QuoteSerializer.hpp"
#include "TimeUtils.hpp"

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

    void init(const std::string &symbol, const QuantCrypto::Quote::ExchangeT &exchange)
    {
        const std::string date = Util::Time::getDate();
        const std::string exchangeName = getExchange(exchange);
        const auto symbolWithExchange = symbol + "." + exchangeName;

        const std::string path = [&]() {
            std::string fileName = getTypeFodlerName() + "/" + exchangeName + "/" + symbolWithExchange + "/" + date + ".txt";
            if (root_.empty()) {
                return fileName;
            }
            return root_ + "/" + fileName;
        }();

        try {
            std::string dirName = getDirname(path);
            std::filesystem::create_directories(dirName);
            std::lock_guard lk(mapLock_);
            auto &symbolWriterMap = fileWriterMap_[exchange];
            auto iter = symbolWriterMap.try_emplace(symbol, path, std::ios_base::app);
            if (!iter.second) {
                logger_.info("Already init, symbol={}, exchange={}", symbol, exchangeName);
                return;
            }
            logger_.info("New file: {}", path);
            auto &symbol = iter.first->first;
            auto &fileWriter = iter.first->second;

            if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::MarketBook>) {
                QuantCrypto::Quote::QuoteApi::onNewBook.subscribe([&fileWriter, &initSymbol = symbol, initExchange = exchange](auto &exchange, auto &book) {
                    if (exchange != initExchange) {
                        return;
                    }
                    if (book.header_.symbol_ != initSymbol) {
                        return;
                    }
                    fileWriter << book << "\n";
                });
            }
            if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::Trade>) {
                QuantCrypto::Quote::QuoteApi::onNewTrade.subscribe([&fileWriter, &initSymbol = symbol, initExchange = exchange](auto &exchange, auto &trade) {
                    static int count = 0;
                    if (exchange != initExchange) {
                        return;
                    }
                    if (trade.header_.symbol_ != initSymbol) {
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
                QuantCrypto::Quote::QuoteApi::onNewKline.subscribe([&fileWriter, &initSymbol = symbol, initExchange = exchange](auto &exchange, auto &kline) {
                    static int count = 0;
                    if (exchange != initExchange) {
                        return;
                    }
                    if (kline.header_.symbol_ != initSymbol) {
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
    constexpr std::string getTypeFodlerName()
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

    std::string getExchange(const QuantCrypto::Quote::ExchangeT &exchange)
    {
        if (exchange == QuantCrypto::Quote::ExchangeT::ByBit) {
            return "Bybit";
        }
        if (exchange == QuantCrypto::Quote::ExchangeT::Binance) {
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