/************************************************
 *  partial_sum_test.cpp
 *  ESA++
 *
 *  Copyright (c) 2014-2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <gtest/gtest.h>

#include "../src/bit_vector.hpp"

using esapp::impl::bit_vector;

typedef bit_vector<2> bitmap;

void construct_bitmap(bitmap &bits)
{
    bits.insert(0, true);   // [1]
    bits.insert(1, false);  //  1  [0]
    bits.insert(0, false);  // [0]  1   0
    bits.insert(1, false);  //  0  [0]  1   0
    bits.insert(2, true);   //  0   0  [1]  1   0
    bits.insert(5, true);   //  0   0   1   1   0  [1]
    bits.insert(3, false);  //  0   0   1  [0]  1   0   1
    bits.insert(6, false);  //  0   0   1   0   1   0  [0]  1
    bits.insert(3, true);   //  0   0   1  [1]  0   1   0   0   1
    bits.insert(9, true);   //  0   0   1   1   0   1   0   0   1  [1]
    bits.insert(10, false); //  0   0   1   1   0   1   0   0   1   1  [0]
    bits.insert(0, true);   // [1]  0   0   1   1   0   1   0   0   1   1   0
}

TEST(BitVectorTest, EmptyBitVector)
{
    bitmap bits;
    EXPECT_EQ(bits.count(), 0);
    EXPECT_EQ(bits.size(), 0);
}

TEST(BitVectorTest, InsertFirstBit)
{
    bitmap bits1;
    bits1.insert(0, true);
    EXPECT_EQ(bits1.count(), 1);
    EXPECT_EQ(bits1.size(), 1);

    bitmap bits2;
    bits2.insert(0, false);
    EXPECT_EQ(bits2.count(), 0);
    EXPECT_EQ(bits2.size(), 1);
}

TEST(BitVectorTest, EraseLastBit)
{
    bitmap bits1;
    bits1.insert(0, true);
    auto b1 = bits1.erase(0);
    EXPECT_EQ(bits1.count(), 0);
    EXPECT_EQ(bits1.size(), 0);
    EXPECT_TRUE(b1);

    bitmap bits2;
    bits2.insert(0, false);
    auto b2 = bits2.erase(0);
    EXPECT_EQ(bits2.count(), 0);
    EXPECT_EQ(bits2.size(), 0);
    EXPECT_FALSE(b2);
}

TEST(BitVectorTest, InsertMoreBit)
{
    bitmap bits;
    construct_bitmap(bits);
    EXPECT_EQ(bits.count(), 6);
    EXPECT_EQ(bits.size(), 12);
}

TEST(BitVectorTest, EraseMoreBit)
{
    bitmap bits;
    construct_bitmap(bits);
    EXPECT_FALSE(bits.erase(5));    // 1 0 0 1 1 1 0 0 1 1 0
    EXPECT_TRUE(bits.erase(0));     // 0 0 1 1 1 0 0 1 1 0
    EXPECT_FALSE(bits.erase(6));    // 0 0 1 1 1 0 1 1 0
    EXPECT_FALSE(bits.erase(5));    // 0 0 1 1 1 1 1 0
    EXPECT_TRUE(bits.erase(3));     // 0 0 1 1 1 1 0
    EXPECT_FALSE(bits.erase(6));    // 0 0 1 1 1 1
    EXPECT_EQ(bits.count(), 4);
    EXPECT_EQ(bits.size(), 6);
}

TEST(BitVectorTest, AccessBits)
{
    bitmap bits;
    construct_bitmap(bits);
    EXPECT_TRUE(bits[0]);
    EXPECT_FALSE(bits[1]);
    EXPECT_FALSE(bits[2]);
    EXPECT_TRUE(bits[3]);
    EXPECT_TRUE(bits[4]);
    EXPECT_FALSE(bits[5]);
    EXPECT_TRUE(bits[6]);
    EXPECT_FALSE(bits[7]);
    EXPECT_FALSE(bits[8]);
    EXPECT_TRUE(bits[9]);
    EXPECT_TRUE(bits[10]);
    EXPECT_FALSE(bits[11]);
}

// TODO: test cases for set/reset/rank/select
