#include <cstddef>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <unordered_set>

#include "ThreadUtil.hpp"

using namespace Util::Thread::ThreadUtil;

TEST(ThreadUtilTest, ThreadName)
{
    std::string thraedName = "simple-test";
    setThreadName(thraedName);

    const auto ret = getThreadName();
    EXPECT_EQ(ret, thraedName);
}

TEST(ThreadUtilTest, MultiThreadName)
{
    std::vector<std::string> threadNames{"thread1", "thread2", "thread3", "thread4", "thread5"};
    std::vector<std::thread> threads;
    std::vector<std::string> result;
    std::mutex mtx;


    for (size_t i = 0; i < threadNames.size(); ++i) {
        threads.emplace_back([&threadNames, &result, i, &mtx]() {
            std::string thraedName = threadNames[i];

            setThreadName(thraedName);

            const auto name = getThreadName();
            {
                std::lock_guard lock(mtx);
                result.emplace_back(name);
            }
        });
    }
    for (auto &thread : threads) {
        thread.join();
    }

    EXPECT_EQ(result.size(), threadNames.size());

    std::unordered_set<std::string> threadNameSet(threadNames.begin(), threadNames.end());

    for (size_t i = 0; i < threadNames.size(); ++i) {
        EXPECT_EQ(threadNameSet.count(result[i]), 1);
        threadNameSet.erase(result[i]);
    }

    EXPECT_EQ(threadNameSet.empty(), true);
}