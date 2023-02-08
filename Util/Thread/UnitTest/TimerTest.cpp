#include <chrono>
#include <cstddef>
#include <gtest/gtest.h>
#include <string>
#include <thread>

#include "Timer.hpp"

using namespace Util::Thread;

TEST(TimerTest, StartAfterPeriodic)
{
    Timer timer;
    
    unsigned int count = 0;
    timer.periodic([&count](){
        count++;
    }, 1000);
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timer.stop();
    EXPECT_TRUE(count >= 10);
}

TEST(TimerTest, StartBeforePeriodic)
{
    Timer timer;
    
    unsigned int count = 0;
    
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    timer.periodic([&count](){
        count++;
    }, 1000);

    std::this_thread::sleep_for(std::chrono::milliseconds(11));
    timer.stop();
    EXPECT_TRUE(count >= 10);
}

TEST(TimerTest, InvokeNumTest)
{
    Timer timer;
    
    unsigned int count = 0;
    unsigned int count2 = 0;
    timer.periodic([&count](){
        count++;
    }, 1000, 5);

    timer.periodic([&count2](){
        count2++;
    }, 1000, 6);

    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timer.stop();
    
    EXPECT_EQ(5, count);
    EXPECT_EQ(6, count2);
}