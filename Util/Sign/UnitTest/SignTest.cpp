#include <gtest/gtest.h>
#include <string>
#include <map>

#include "BybitSignTool.hpp"

using namespace Util::Sign;

TEST(BybitSignTest, HmacSha256Test)
{
    static const std::string MockApiSecret = "dkl3l5k4i26jhsehiklfgjpoihiosghiopsh";
    static const std::string TestStr = "test-query-string";

    std::string sign = hmac<SHA256>(TestStr, MockApiSecret);

    EXPECT_EQ("b77be1457a66446d9ba547d0dba70fcb1c5f5f27bfac7a264e4330999e261079", sign);
}

TEST(BybitSignTest, SignHttpRequestTest)
{
    static const std::string MockApiKey = "JIDOjapfjOIjood11L";
    static const std::string MockApiSecret = "dkl3l5k4i26jhsehiklfgjpoihiosghiopsh";

    std::map<std::string, std::string> params{
        {"api_key", MockApiKey},
        {"price", "1.0"},
        {"qty", "100"},
        {"side", "BUY"},
        {"symbol", "BITUSDT"},
        {"type", "LIMIT"},
        {"timeInForce", "IOC"},
    };

    std::string signQuery = BybitSignTool::signHttpReq(params, MockApiSecret);

    EXPECT_EQ("api_key=JIDOjapfjOIjood11L&price=1.0&qty=100&side=BUY&symbol=BITUSDT&timeInForce=IOC&type=LIMIT&sign=1894512a258140b38f669373e285a9e5dd947fe2d9cbd57d69b40980e793c93c",
              signQuery);
}