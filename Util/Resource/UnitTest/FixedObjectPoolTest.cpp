#include "FixedObjectPool.hpp"

#include <gtest/gtest.h>

using namespace Util::Resource;

struct Obj
{
    Obj(long long no)
        : sequenceNo(no)
    {
    }
    long long sequenceNo;
};

TEST(ResourceTest, FixedObjectPoolBasic)
{
    FixedObjectPool<Obj> pool{4};

    Obj *p1 = pool.construct(1);
    Obj *p2 = pool.construct(2);
    Obj *p3 = pool.construct(3);
    Obj *p4 = pool.construct(4);
    Obj *p5 = pool.construct(5);

    // Value test
    EXPECT_EQ(p1->sequenceNo, 1);
    EXPECT_EQ(p2->sequenceNo, 2);
    EXPECT_EQ(p3->sequenceNo, 3);
    EXPECT_EQ(p4->sequenceNo, 4);
    EXPECT_EQ(p5, nullptr);

    // Memory Address
    EXPECT_EQ((char *)&*p2 - (char *)&*p1, sizeof(Obj));
    EXPECT_EQ((char *)&*p3 - (char *)&*p2, sizeof(Obj));
    EXPECT_EQ((char *)&*p4 - (char *)&*p3, sizeof(Obj));
}

TEST(ResourceTest, FixedObjectPoolResource)
{
    FixedObjectPool<Obj> pool{4};

    Obj *p1 = pool.construct(1);
    pool.recycle(p1);
    Obj *p2 = pool.construct(2);

    EXPECT_EQ(&*p1, &*p2);  // Reuse same memory

    EXPECT_EQ(p1->sequenceNo, 2);
    EXPECT_EQ(p2->sequenceNo, 2);
}

TEST(ResourceTest, FixedObjectPoolIsFromPool)
{
    FixedObjectPool<Obj> pool{4};

    Obj *p1 = pool.construct(1);
    Obj *p2 = pool.construct(2);
    Obj *p3 = pool.construct(3);
    Obj *p4 = pool.construct(4);

    EXPECT_TRUE(pool.isFromPool(p1));
    EXPECT_TRUE(pool.isFromPool(p2));
    EXPECT_TRUE(pool.isFromPool(p3));
    EXPECT_TRUE(pool.isFromPool(p4));

    Obj *p5 = new Obj(-1);
    EXPECT_FALSE(pool.isFromPool(p5));
    delete p5;
}