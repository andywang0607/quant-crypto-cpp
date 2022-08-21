#ifndef __QUOTEWRITTER_H__
#define __QUOTEWRITTER_H__

#include "QuoteApi.hpp"
#include "QuoteData.hpp"
#include "QuoteSerializer.hpp"
#include "TimeUtils.hpp"

#include <exception>
#include <filesystem>
#include <fstream>
#include <type_traits>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace QuantCrypto::QuoteReceiver::QuoteUtil {

template <typename QuoteType>
class QuoteWritter
{
public:
    QuoteWritter(const nlohmann::json &config)
        : root_(config["QuoteReceiver"]["root"].get<std::string>())
    {
    }

    void init(const std::string &symbol, const QuoteType &quote)
    {
        const std::string date = Util::Time::getDate();
        const std::string exchange = getExchange(quote);

        const std::string path = [&]() {
            if (root_.empty()) {
                return getTypeFodlerName(quote) + "/" + exchange + "/" + symbol + "/" + date + ".txt";
            }
            return root_ + "/" + getTypeFodlerName(quote) + "/" + exchange + "/" + symbol + "/" + date + ".txt";
        }();
        spdlog::info("[QuoteWritter] New file: {}", path);

        try {
            std::string dirName = getDirname(path);
            std::filesystem::create_directories(dirName);

            auto &fileWriter = fileWriterMap_.try_emplace(symbol, path, std::ios_base::app).first->second;

            if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::MarketBook>) {
                QuantCrypto::Quote::QuoteApi::onNewBook.subscribe([&fileWriter](auto &book) {
                    const auto symbol = book.header_.symbol_;
                    fileWriter << book << "\n";
                });
            }
            if constexpr (std::is_same_v<QuoteType, QuantCrypto::Quote::Trade>) {
                QuantCrypto::Quote::QuoteApi::onNewTrade.subscribe([&fileWriter](auto &trade) {
                    static int count = 0;
                    const auto symbol = trade.header_.symbol_;
                    fileWriter << trade << "\n";
                    if (count++ >= 5) {
                        fileWriter.flush();
                        count = 0;
                    }
                });
            }
        } catch (std::exception &e) {
            spdlog::info("[QuoteWritter] exception: {}", e.what());
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
};
} // namespace QuantCrypto::QuoteReceiver::QuoteUtil

#endif // __QUOTEWRITTER_H__