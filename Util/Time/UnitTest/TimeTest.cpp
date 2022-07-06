#include <gtest/gtest.h>

#include "TimeUtils.hpp"

TEST(TimeUtilTest, toTimestamp)
{
    std::string testStr = "2020-03-30T02:21:06.000Z";
    const auto timestamp = Util::Time::toTimestamp(testStr);

    EXPECT_EQ(1585534866000, timestamp);
}