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
#include <type_traits>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace QuantCrypto::QuoteReceiver::QuoteUtil {

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
        logger_.info("New file: {}", path);

        try {
            std::string dirName = getDirname(path);
            std::filesystem::create_directories(dirName);

            auto iter = fileWriterMap_.try_emplace(symbol, path, std::ios_base::app).first;
            auto &symbol = iter->first;
            auto &fileWriter = iter->second;

            if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::MarketBook>) {
                QuantCrypto::Quote::QuoteApi::onNewBook.subscribe([&symbol, &fileWriter](auto &exchange, auto &book) {
                    const auto receivedSymbol = book.header_.symbol_;
                    if (receivedSymbol != symbol) {
                        return;
                    }
                    fileWriter << book << "\n";
                });
            }
            if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::Trade>) {
                QuantCrypto::Quote::QuoteApi::onNewTrade.subscribe([&symbol, &fileWriter](auto &exchange, auto &trade) {
                    static int count = 0;
                    const auto receivedSymbol = trade.header_.symbol_;
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
        return "";
    }

    std::string root_;
    std::unordered_map<std::string, std::ofstream> fileWriterMap_;
    Util::Log::Logger logger_;
};
} // namespace QuantCrypto::QuoteReceiver::QuoteUtil

#endif // __QUOTEWRITTER_H__