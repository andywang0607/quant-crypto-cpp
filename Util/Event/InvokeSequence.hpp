#ifndef __INVOKESEQUENCE_H__
#define __INVOKESEQUENCE_H__

#include <functional>
#include <iterator>
#include <utility>
#include <vector>

namespace Util::Event {

class SequenceInvokePolicy
{
public:
    template <typename IterType, typename F>
    static auto for_each(const IterType &begin, const IterType &end, F &&f_)
    {
        std::for_each(begin, end, [&](auto &f) {
            if (f) {
                std::forward<F>(f_)(f);
            }
        });
    }

    template <typename IterType, typename... Args>
    static auto invoke(const IterType &begin, const IterType &end, Args &&... args)
    {
        using Invokable = typename std::iterator_traits<IterType>::value_type;
        if constexpr (std::is_same_v<bool, std::invoke_result_t<Invokable, Args...>>) {
            bool result = true;
            for_each(begin, end, [&](const auto &f) {
                result &= f(std::forward<Args>(args)...);
            });

            return result;
        } else {
            for_each(begin, end, [&](const auto &f) {
                f(std::forward<Args>(args)...);
            });
        }
    }
};

template <typename ReturnType, typename InvokePolicy = SequenceInvokePolicy, typename... ArgTypes>
class InvokeSequence;

template <typename ReturnType, typename InvokePolicy, typename... Args>
class InvokeSequence<ReturnType(Args...), InvokePolicy>
{
public:
    using InvokableType = std::function<ReturnType(Args...)>;

    template <typename F, typename = std::enable_if_t<std::is_invocable_v<F, Args...>>>
    [[nodiscard]] auto operator+=(F &&f)
    {
        auto iter = sequence_.begin();
        for (; iter != sequence_.end(); ++iter) {
            if (*iter != nullptr) {
                continue;
            }
            *iter = std::forward<F>(f);
            break;
        }

        if (iter == sequence_.end()) {
            sequence_.emplace_back(std::forward<F>(f));
            iter = std::prev(sequence_.end());
        }

        const auto index = std::distance(sequence_.begin(), iter);

        return [iter]() {
            *iter = nullptr;
        };
    }

    auto operator()(Args &&... args)
    {
        return InvokePolicy::invoke(sequence_.begin(), sequence_.end(), std::forward<Args>(args)...);
    }

    std::vector<InvokableType> sequence_;
};

class Unsubscriber
{
public:
    using UnsucribeType = std::function<void()>;

    template <typename F, typename = std::enable_if_t<std::is_invocable_v<F>>>
    void operator+=(F &&f)
    {
        unsubscribers_.emplace_back(std::forward<F>(f));
    }

    void unsubscribeAll()
    {
        std::for_each(unsubscribers_.begin(), unsubscribers_.end(), [](const auto &unsub) {
            unsub();
        });
        unsubscribers_.clear();
    }

    ~Unsubscriber()
    {
        unsubscribeAll();
    }

    std::vector<UnsucribeType> unsubscribers_;
};

} // namespace Util::Event

#endif // __INVOKESEQUENCE_H__