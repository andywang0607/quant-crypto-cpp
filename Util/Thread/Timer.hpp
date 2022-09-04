#ifndef __UTIL_TIMER_H__
#define __UTIL_TIMER_H__

#include "ThreadUtil.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

namespace Util::Thread {

class TimerEvent
{
public:
    using ClockType = std::chrono::system_clock;
    using TimePoint = typename ClockType::time_point;

    TimerEvent() = default;
    ~TimerEvent() = default;

    struct ExpiredEvent
    {
        template <typename Func>
        ExpiredEvent(Func &&callback, unsigned long long usInterval, TimePoint timePoint, long long invokeNum)
            : onExpired(std::forward<Func>(callback))
            , interval(std::chrono::microseconds(usInterval))
            , invokeTime(timePoint)
            , invokeNum(invokeNum)
        {
        }

        friend bool operator<(ExpiredEvent const &lhs, ExpiredEvent const &rhs)
        {
            return lhs.invokeTime > rhs.invokeTime;
        }

        friend bool operator<(std::shared_ptr<ExpiredEvent> const &lhs, std::shared_ptr<ExpiredEvent> const &rhs)
        {
            return *lhs < *rhs;
        }

        std::function<void()> onExpired;
        std::chrono::microseconds interval;
        TimePoint invokeTime;
        long long invokeNum;
    };

    template <typename Func, typename = std::enable_if_t<std::is_invocable_v<Func>>>
    auto registerPeriodicEvent(Func &&callback, unsigned long long usInterval, long long invokeNum)
    {
        std::lock_guard lk(mutex_);
        auto expiredTimeEvent = std::make_shared<ExpiredEvent>(std::forward<Func>(callback), usInterval, ClockType::now(), invokeNum);
        eventQueue_.push(expiredTimeEvent);
    }

    void checkEvent()
    {
        auto now = ClockType::now();
        while (!eventQueue_.empty()) {
            std::lock_guard lk(mutex_);
            auto expiredEvent = eventQueue_.top();
            if (expiredEvent->invokeTime > now) {
                break;
            }
            expiredEvent->onExpired();
            eventQueue_.pop();

            if (expiredEvent->invokeNum == -1 || --expiredEvent->invokeNum > 0) {
                expiredEvent->invokeTime = now + expiredEvent->interval;
                eventQueue_.push(expiredEvent);
            }
        }
    }

private:
    std::priority_queue<std::shared_ptr<ExpiredEvent>, std::vector<std::shared_ptr<ExpiredEvent>>> eventQueue_;
    mutable std::mutex mutex_;
};

class Timer
{
public:
    Timer() = default;
    Timer(const std::string &threadName)
        : threadName_(threadName)
    {
    }

    template <typename Func, typename = std::enable_if_t<std::is_invocable_v<Func>>>
    auto periodic(Func &&callback, unsigned long long usInterval, long long invokeNum = -1)
    {
        timerEvent_.registerPeriodicEvent(std::forward<Func>(callback), usInterval, invokeNum);
    }

    void start()
    {
        isRunning_.store(true, std::memory_order::memory_order_release);
        timerThread_.emplace_back([this]() {
            if (!threadName_.empty()) {
                ThreadUtil::setThreadName(threadName_);
            }
            while (isRunning_.load(std::memory_order::memory_order_acquire)) {
                timerEvent_.checkEvent();

                std::this_thread::sleep_for(std::chrono::nanoseconds(20));
            }
        });
    }

    void stop()
    {
        isRunning_.store(false, std::memory_order::memory_order_release);
        for (auto &thread : timerThread_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        timerThread_.clear();
    }

private:
    std::atomic_bool isRunning_;
    std::vector<std::thread> timerThread_;
    TimerEvent timerEvent_;
    std::string threadName_;
};

} // namespace Util::Thread
#endif // __UTIL_TIMER_H__