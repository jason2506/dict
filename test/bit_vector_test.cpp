/************************************************
 *  bit_vector_test.cpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <utility>

#include <gtest/gtest.h>

#include "../src/bit_vector.hpp"

using desa::impl::bit_vector;

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

void construct_bitmap_with_erasure(bitmap &bits)
{
    construct_bitmap(bits);
    EXPECT_FALSE(bits.erase(5));    // 1 0 0 1 1 1 0 0 1 1 0
    EXPECT_TRUE(bits.erase(0));     // 0 0 1 1 1 0 0 1 1 0
    EXPECT_FALSE(bits.erase(6));    // 0 0 1 1 1 0 1 1 0
    EXPECT_FALSE(bits.erase(5));    // 0 0 1 1 1 1 1 0
    EXPECT_TRUE(bits.erase(3));     // 0 0 1 1 1 1 0
    EXPECT_FALSE(bits.erase(6));    // 0 0 1 1 1 1
}

TEST(BitVectorTest, EmptyBitVector)
{
    bitmap bits;
    EXPECT_EQ(0, bits.count());
    EXPECT_EQ(0, bits.size());
}

TEST(BitVectorTest, InsertFirstBit)
{
    bitmap bits1;
    bits1.insert(0, true);
    EXPECT_EQ(1, bits1.count());
    EXPECT_EQ(1, bits1.size());

    bitmap bits2;
    bits2.insert(0, false);
    EXPECT_EQ(0, bits2.count());
    EXPECT_EQ(1, bits2.size());
}

TEST(BitVectorTest, EraseLastBit)
{
    bitmap bits1;
    bits1.insert(0, true);
    auto b1 = bits1.erase(0);
    EXPECT_EQ(0, bits1.count());
    EXPECT_EQ(0, bits1.size());
    EXPECT_TRUE(b1);

    bitmap bits2;
    bits2.insert(0, false);
    auto b2 = bits2.erase(0);
    EXPECT_EQ(0, bits2.count());
    EXPECT_EQ(0, bits2.size());
    EXPECT_FALSE(b2);
}

TEST(BitVectorTest, InsertMoreBits)
{
    bitmap bits;
    construct_bitmap(bits);
    EXPECT_EQ(6, bits.count());
    EXPECT_EQ(12, bits.size());
}

TEST(BitVectorTest, EraseMoreBit)
{
    bitmap bits;
    construct_bitmap_with_erasure(bits);
    EXPECT_EQ(4, bits.count());
    EXPECT_EQ(6, bits.size());
}

TEST(BitVectorTest, AccessBitsAfterInsertion)
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

TEST(BitVectorTest, AccessBitsAfterErasure)
{
    bitmap bits;
    construct_bitmap_with_erasure(bits);
    EXPECT_FALSE(bits[0]);
    EXPECT_FALSE(bits[1]);
    EXPECT_TRUE(bits[2]);
    EXPECT_TRUE(bits[3]);
    EXPECT_TRUE(bits[4]);
    EXPECT_TRUE(bits[5]);
}

TEST(BitVectorTest, SetAndReset)
{
    bitmap bits;
    construct_bitmap(bits);

    bits.set(2);
    EXPECT_TRUE(bits[2]);
    EXPECT_EQ(7, bits.count());
    EXPECT_EQ(12, bits.size());

    bits.reset(2);
    EXPECT_FALSE(bits[2]);
    EXPECT_EQ(6, bits.count());
    EXPECT_EQ(12, bits.size());

    bits.set(10, false);
    EXPECT_FALSE(bits[10]);
    EXPECT_EQ(5, bits.count());
    EXPECT_EQ(12, bits.size());

    bits.set(11, true);
    EXPECT_TRUE(bits[11]);
    EXPECT_EQ(6, bits.count());
    EXPECT_EQ(12, bits.size());
}

TEST(BitVectorTest, RankBits)
{
    bitmap bits;
    construct_bitmap(bits);

    EXPECT_EQ(1, bits.rank(0, true));
    EXPECT_EQ(1, bits.rank(1, true));
    EXPECT_EQ(1, bits.rank(2, true));
    EXPECT_EQ(2, bits.rank(3, true));
    EXPECT_EQ(3, bits.rank(4, true));
    EXPECT_EQ(3, bits.rank(5, true));
    EXPECT_EQ(4, bits.rank(6, true));
    EXPECT_EQ(4, bits.rank(7, true));
    EXPECT_EQ(4, bits.rank(8, true));
    EXPECT_EQ(5, bits.rank(9, true));
    EXPECT_EQ(6, bits.rank(10, true));
    EXPECT_EQ(6, bits.rank(11, true));

    EXPECT_EQ(0, bits.rank(0, false));
    EXPECT_EQ(1, bits.rank(1, false));
    EXPECT_EQ(2, bits.rank(2, false));
    EXPECT_EQ(2, bits.rank(3, false));
    EXPECT_EQ(2, bits.rank(4, false));
    EXPECT_EQ(3, bits.rank(5, false));
    EXPECT_EQ(3, bits.rank(6, false));
    EXPECT_EQ(4, bits.rank(7, false));
    EXPECT_EQ(5, bits.rank(8, false));
    EXPECT_EQ(5, bits.rank(9, false));
    EXPECT_EQ(5, bits.rank(10, false));
    EXPECT_EQ(6, bits.rank(11, false));
}

TEST(BitVectorTest, RankAndAccessBits)
{
    bitmap bits;
    construct_bitmap(bits);

    typedef ::std::pair<bitmap::size_type, bitmap::value_type> rb_pair;
    EXPECT_EQ(rb_pair(1, true), bits.rank(0));
    EXPECT_EQ(rb_pair(1, false), bits.rank(1));
    EXPECT_EQ(rb_pair(2, false), bits.rank(2));
    EXPECT_EQ(rb_pair(2, true), bits.rank(3));
    EXPECT_EQ(rb_pair(3, true), bits.rank(4));
    EXPECT_EQ(rb_pair(3, false), bits.rank(5));
    EXPECT_EQ(rb_pair(4, true), bits.rank(6));
    EXPECT_EQ(rb_pair(4, false), bits.rank(7));
    EXPECT_EQ(rb_pair(5, false), bits.rank(8));
    EXPECT_EQ(rb_pair(5, true), bits.rank(9));
    EXPECT_EQ(rb_pair(6, true), bits.rank(10));
    EXPECT_EQ(rb_pair(6, false), bits.rank(11));
}

TEST(BitVectorTest, SelectBits)
{
    bitmap bits;
    construct_bitmap(bits);

    EXPECT_EQ(0, bits.select(0, true));
    EXPECT_EQ(3, bits.select(1, true));
    EXPECT_EQ(4, bits.select(2, true));
    EXPECT_EQ(6, bits.select(3, true));
    EXPECT_EQ(9, bits.select(4, true));
    EXPECT_EQ(10, bits.select(5, true));

    EXPECT_EQ(1, bits.select(0, false));
    EXPECT_EQ(2, bits.select(1, false));
    EXPECT_EQ(5, bits.select(2, false));
    EXPECT_EQ(7, bits.select(3, false));
    EXPECT_EQ(8, bits.select(4, false));
    EXPECT_EQ(11, bits.select(5, false));
}
