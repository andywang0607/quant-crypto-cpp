#ifndef __TOPIC_H__
#define __TOPIC_H__

#include "InvokeSequence.hpp"
#include <utility>

namespace Util::Event {
template <typename... Args>
class Topic : public InvokeSequence<void(Args...)>
{
public:
    template <typename F, typename = std::enable_if_t<std::is_invocable_v<F, Args...>>>
    constexpr auto subscribe(F &&f)
    {
        return (*this) += std::forward<F>(f);
    }

    constexpr auto publish(Args &&...args)
    {
        (*this)(std::forward<Args>(args)...);
    }
};
} // namespace Util::Event

#endif // __TOPIC_H__