#include <gtest/gtest.h>
#include <string>

#include "InvokeSequence.hpp"

using namespace Util::Event;

TEST(InvokeSequenceTest, Basic)
{
    InvokeSequence<void(int, int)> invokeSeq;

    Unsubscriber unsubcriber;

    int ret = 0;

    unsubcriber += invokeSeq += [&ret](int a, int b) {
        ret += (a + b);
    };

    invokeSeq(123, 456);
    EXPECT_EQ((123 + 456), ret);

    unsubcriber += invokeSeq += [&ret](int a, int b) {
        ret += (a + b);
    };

    invokeSeq(123, 456);
    EXPECT_EQ((123 + 456) * 3, ret);

    unsubcriber.unsubscribeAll();
};

TEST(InvokeSequenceTest, MultiInvoke)
{
    InvokeSequence<void()> invokeSeq;

    Unsubscriber unsubcriber;

    int counter1 = 0;
    int counter2 = 0;
    int counter3 = 0;

    unsubcriber += invokeSeq += [&counter1]() {
        counter1++;
    };

    unsubcriber += invokeSeq += [&counter2]() {
        counter2++;
    };

    unsubcriber += invokeSeq += [&counter3]() {
        counter3++;
    };

    invokeSeq();

    EXPECT_EQ(1, counter1);
    EXPECT_EQ(1, counter2);
    EXPECT_EQ(1, counter3);
};

TEST(InvokeSequenceTest, BoleanReturn1)
{
    InvokeSequence<bool()> invokeSeq;

    Unsubscriber unsubcriber;

    int test = 2;

    unsubcriber += invokeSeq += [&test]() {
        return test > 1;
    };

    unsubcriber += invokeSeq += [&test]() {
        return test < 3;
    };

    EXPECT_EQ(true, invokeSeq());
};

TEST(InvokeSequenceTest, BoleanReturn2)
{
    InvokeSequence<bool()> invokeSeq;

    Unsubscriber unsubcriber;

    int test = 2;

    unsubcriber += invokeSeq += [&test]() {
        return test > 1;
    };

    unsubcriber += invokeSeq += [&test]() {
        return test > 3;
    };

    EXPECT_EQ(false, invokeSeq());
};