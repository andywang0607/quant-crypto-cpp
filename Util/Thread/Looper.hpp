#ifndef __UTIL_LOOPER_H__
#define __UTIL_LOOPER_H__

#include <atomic>
#include <thread>
#include <vector>

#include "InvokeSequence.hpp"
#include "ThreadUtil.hpp"

namespace Util::Thread {

template<size_t YieldInterval = 1024>
class Looper
{
public:
    Looper() = default;
    Looper(const std::string &threadName)
        : threadName_(threadName)
    {
    }

    ~Looper()
    {
        stop();
    }

    template <typename Func, typename = std::enable_if_t<std::is_invocable_v<Func>>>
    void run(Func &&func)
    {
        unsubscriber_ += invokables_ += std::forward<Func>(func);
        run();
    }

    void run()
    {
        isRunning_.store(true, std::memory_order::memory_order_release);
        threads_.emplace_back([this]() {
            ThreadUtil::setThreadName(threadName_);
            thread_local int yieldCount = 0;
            while (isRunning_.load(std::memory_order::memory_order_acquire)) {
                invokables_();
                if (++yieldCount >= YieldInterval) {
                    std::this_thread::yield();
                    yieldCount = 0;
                }
            }
        });
    }

    void stop()
    {
        isRunning_.store(false, std::memory_order::memory_order_release);
        for (auto &thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        threads_.clear();
    }

    template <typename Func, typename = std::enable_if_t<std::is_invocable_v<Func>>>
    void addTask(Func &&func)
    {
        unsubscriber_ += invokables_ += std::forward<Func>(func);
    }

private:
    Util::Event::InvokeSequence<void()> invokables_;
    Util::Event::Unsubscriber unsubscriber_;
    std::vector<std::thread> threads_;
    std::atomic_bool isRunning_;
    std::string threadName_;
    
};
} // namespace Util::Thread

#endif // __UTIL_LOOPER_H__