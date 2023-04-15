#ifndef __UTIL_QURIER_H__
#define __UTIL_QURIER_H__

#include "Logger.hpp"

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

namespace Util::Qurier {
template <typename ResultType>
class Querier
{
public:
    Querier(const std::string &name)
        : name_(name)
        , isSuccess_(false)
        , logger_(name_)
    {
    }

    template <typename QueryFunc, typename = std::enable_if_t<std::is_invocable_v<QueryFunc>>>
    bool query(QueryFunc &&query)
    {
        return doQuery(std::forward<QueryFunc>(query));
    }

    template <typename IsSuccessFunc, typename = std::enable_if_t<std::is_invocable_v<IsSuccessFunc, const ResultType &>>>
    void onFinish(const ResultType &result, IsSuccessFunc &&isSuccessFunc)
    {
        logger_.info("query {} finished", name_);
        const bool isSuccess = isSuccessFunc(result);
        isSuccess_ = isSuccess;
        isFinish_.store(true, std::memory_order_release);
    }

    using OnResultFuncType = std::function<void(const ResultType &)>;
    OnResultFuncType onResult;

private:
    template <typename QueryFunc>
    bool doQuery(const QueryFunc &&query)
    {
        logger_.info("querying {}", name_);
        isFinish_.store(false, std::memory_order_release);
        if (!query()) {
            logger_.info("query {} failed", name_);
            return false;
        }

        using ClockType = std::chrono::steady_clock;
        const auto timeoutPoint = ClockType::now() + Timeout;
        while (!isFinish_.load(std::memory_order_relaxed)) {
            const auto now = ClockType::now();
            if (now >= timeoutPoint) {
                logger_.warn("query Timeout");
                break;
            }
            std::this_thread::yield();
        }
        const auto isFinish = isFinish_.load(std::memory_order_acquire);
        logger_.info("isFinish_={}, success={}", isFinish, isSuccess_);
        return isFinish && isSuccess_;
    }

    std::string name_;
    bool isSuccess_;
    Util::Log::Logger logger_;
    std::atomic_bool isFinish_ = ATOMIC_VAR_INIT(false);

    static constexpr std::chrono::seconds Timeout = std::chrono::seconds(3);
};
} // namespace Util::Qurier

#endif // __UTIL_QURIER_H__