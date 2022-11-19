#include <gtest/gtest.h>

#include "StringUtil.hpp"

TEST(StringUtilTest, toUpper)
{
    std::string testStr = "symbol";
    Util::StringUtil::toUpperCase(testStr);

    EXPECT_EQ("SYMBOL", testStr);
}

TEST(StringUtilTest, toLower)
{
    std::string testStr = "SYMBOL";
    Util::StringUtil::toLowerCase(testStr);

    EXPECT_EQ("symbol", testStr);
}