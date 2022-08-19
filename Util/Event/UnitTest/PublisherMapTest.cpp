#include <gtest/gtest.h>
#include <string>

#include "PublisherMap.hpp"

using namespace Util::Event;

TEST(PublisherMapTest, BasicPublishTest)
{
    PublisherMap<std::string, int> map;
    Unsubscriber unsubcriber;

    int count = 0;
    bool ret = 0;

    std::string lastKey = "USTETH";
    int lastValue = 800;

    unsubcriber += map.subscribe([&](auto &key, auto &value) {
        ++count;
        EXPECT_EQ(key, lastKey);
        EXPECT_EQ(value, lastValue);
    });

    map.emplace(std::piecewise_construct, std::forward_as_tuple(lastKey), std::forward_as_tuple(lastValue));
    EXPECT_EQ(count, 1);
};

TEST(PublisherMapTest, BasicPublishTest2)
{
    PublisherMap<std::string, int> map;
    Unsubscriber unsubcriber;

    int count = 0;
    bool ret = 0;

    std::string lastKey = "USTETH";
    int lastValue = 800;

    unsubcriber += map.subscribe([&](auto &key, auto &value) {
        ++count;
        EXPECT_EQ(key, lastKey);
        EXPECT_EQ(value, lastValue);
    });

    map.try_emplace(lastKey, lastValue);
    EXPECT_EQ(count, 1);
};
