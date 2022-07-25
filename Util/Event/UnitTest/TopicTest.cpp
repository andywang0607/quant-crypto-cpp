#include <gtest/gtest.h>
#include <string>

#include "Topic.hpp"

using namespace Util::Event;

TEST(TopicTest, MemberFunction)
{
    Topic<int, int> topic;

    Unsubscriber unsubcriber;

    int ret = 0;

    unsubcriber += topic.subscribe([&ret](int x, int y) {
        ret += (x + y);
    });

    topic.publish(1, 2);
    EXPECT_EQ(1 + 2, ret);

    unsubcriber += topic.subscribe([&ret](int x, int y) {
        ret += (x * y);
    });

    topic.publish(3, 4);
    EXPECT_EQ((1 + 2) + (3 + 4) + (3 * 4), ret);
};

TEST(TopicTest, Operator)
{
    Topic<int, int> topic;

    Unsubscriber unsubcriber;

    int ret = 0;

    unsubcriber += topic += [&ret](int x, int y) {
        ret += (x + y);
    };

    topic.publish(1, 2);
    EXPECT_EQ(1 + 2, ret);

    unsubcriber += topic += [&ret](int x, int y) {
        ret += (x * y);
    };

    topic.publish(3, 4);
    EXPECT_EQ((1 + 2) + (3 + 4) + (3 * 4), ret);
};