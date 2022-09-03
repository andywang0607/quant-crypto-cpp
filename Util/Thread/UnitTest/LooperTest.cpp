#include <cstddef>
#include <gtest/gtest.h>
#include <string>

#include "Looper.hpp"

using namespace Util::Thread;

TEST(LooperTest, RunWithFunc)
{
    Looper looper;

    unsigned int count = 0;
    looper.run([&count]() {
        count++;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    looper.stop();
    EXPECT_EQ(true, count > 0);
}

TEST(LooperTest, AddTaskAndRun)
{
    Looper looper;

    unsigned int count = 0;
    looper.addTask([&count]() {
        count++;
    });
    looper.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    looper.stop();
    EXPECT_EQ(true, count > 0);
}

TEST(LooperTest, AddMultiTaskAndRun)
{
    Looper looper;

    unsigned int count = 0;
    looper.addTask([&count]() {
        count++;
    });
    unsigned int count2 = 0;
    looper.addTask([&count2]() {
        count2++;
    });
    looper.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    looper.stop();
    EXPECT_EQ(true, count > 0);
    EXPECT_EQ(true, count2 > 0);
}

TEST(LooperTest, ThreadName)
{
    Looper looper("TestLooper");    // Check it with htop

    unsigned int count = 0;
    looper.addTask([&count]() {
        count++;
    });
    looper.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    looper.stop();
    EXPECT_EQ(true, count > 0);
}
