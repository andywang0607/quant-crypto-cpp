#ifndef __PUBLISHERMAP_H__
#define __PUBLISHERMAP_H__

#include "Topic.hpp"

#include <unordered_map>

namespace Util::Event {

template <typename KeyType, typename ValueType>
class PublisherMap : public std::unordered_map<KeyType, ValueType>
{
public:
    using MapType = std::unordered_map<KeyType, ValueType>;
    using MapType::begin;
    using MapType::end;

    template <typename... Args>
    auto emplace(Args &&... data)
    {
        auto ret = MapType::emplace(std::forward<Args>(data)...);
        if (ret.second) {
            topic_(ret.first->first, ret.first->second);
        }

        return ret;
    }

    template <typename F, typename = std::enable_if_t<std::is_invocable_v<F, KeyType &, ValueType &>>>
    auto subscribe(F &&f)
    {
        std::for_each(begin(), end(), [&f](auto &pair) {
            std::forward<F>(f)(pair.first, pair.second);
        });
        return topic_ += std::forward<F>(f);
    }

private:
    Util::Event::Topic<const KeyType &, ValueType &> topic_;
};
} // namespace Util::Event

#endif // __PUBLISHERMAP_H__