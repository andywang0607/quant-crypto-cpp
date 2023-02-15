#include "Querier.hpp"

#include <future>
#include <string>

#include <gtest/gtest.h>

using namespace Util::Qurier;

TEST(QuerierTest, QuerySuccess)
{
    Querier<std::string> stringQuerier("StrQuerier");

    std::string retMsg;
    stringQuerier.onResult = [&retMsg](const std::string &ret) {
        retMsg = ret;
    };

    std::future<bool> queryFuture = std::async(std::launch::async, [&stringQuerier] {
        return stringQuerier.query([]() {
            // send request success
            return true;
        });
    });

    // Sleep a while for aviod call onFinish before query
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 

    const std::string receivedResult = "success";
    stringQuerier.onFinish(receivedResult, [](const std::string &result) {
        return result == "success";
    });
    stringQuerier.onResult(receivedResult);

    EXPECT_EQ(queryFuture.get(), true); // success && finish
    EXPECT_EQ(retMsg, "success");
}

TEST(QuerierTest, QuerySuccessAndReturnValueIsNotSuccess)
{
    Querier<std::string> stringQuerier("StrQuerier");

    std::string retMsg;
    stringQuerier.onResult = [&retMsg](const std::string &ret) {
        retMsg = ret;
    };

    std::future<bool> queryFuture = std::async(std::launch::async, [&stringQuerier] {
        return stringQuerier.query([]() {
            // send request success
            return true;
        });
    });

    // Sleep a while for aviod call onFinish before query
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 

    const std::string receivedResult = "failed";
    stringQuerier.onFinish(receivedResult, [](const std::string &result) {
        return result == "success";
    });
    stringQuerier.onResult(receivedResult);

    EXPECT_EQ(queryFuture.get(), false); // finish but not success
    EXPECT_EQ(retMsg, "failed");
}

TEST(QuerierTest, QueryFailed)
{
    Querier<std::string> stringQuerier("StrQuerier");

    std::future<bool> queryFuture = std::async(std::launch::async, [&stringQuerier] {
        return stringQuerier.query([]() {
            // send request failed
            return false;
        });
    });

    // Sleep a while for aviod call onFinish before query
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 

    EXPECT_EQ(queryFuture.get(), false); // not success
}

TEST(QuerierTest, QueryTimeout)
{
    Querier<std::string> stringQuerier("StrQuerier");

    std::future<bool> queryFuture = std::async(std::launch::async, [&stringQuerier] {
        return stringQuerier.query([]() {
            // send request success
            return true;
        });
    });

    EXPECT_EQ(queryFuture.get(), false);
}
