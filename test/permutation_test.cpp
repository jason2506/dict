/************************************************
 *  permutation_test.cpp
 *  ESA++
 *
 *  Copyright (c) 2014-2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <gtest/gtest.h>

#include "../src/permutation.hpp"

using esapp::impl::permutation;

void construct_permutation(permutation &pi)
{
    pi.insert(0, 0); // [0]
    pi.insert(1, 0); //  1  [0]
    pi.insert(0, 1); // [1]  2   0
    pi.insert(3, 3); //  1   2   0  [3]
    pi.insert(2, 0); //  2   3  [0]  1   4
    pi.insert(0, 5); // [5]  2   3   0   1   4
}

TEST(PermutationTest, Empty)
{
    permutation pi;
    EXPECT_EQ(0, pi.size());
}

TEST(PermutationTest, Insert)
{
    permutation pi;
    construct_permutation(pi);
    EXPECT_EQ(6, pi.size());
}

TEST(PermutationTest, Erase)
{
    permutation pi;
    construct_permutation(pi);
    pi.erase(1);
    pi.erase(3);
    EXPECT_EQ(4, pi.size());
}

TEST(PermutationTest, AccessAfterInsertion)
{
    permutation pi;
    construct_permutation(pi);
    EXPECT_EQ(5, pi[0]);
    EXPECT_EQ(2, pi[1]);
    EXPECT_EQ(3, pi[2]);
    EXPECT_EQ(0, pi[3]);
    EXPECT_EQ(1, pi[4]);
    EXPECT_EQ(4, pi[5]);
}

TEST(PermutationTest, AccessAfterErasure)
{
    permutation pi;
    construct_permutation(pi);
    pi.erase(1);
    pi.erase(3);
    EXPECT_EQ(3, pi[0]);
    EXPECT_EQ(1, pi[1]);
    EXPECT_EQ(0, pi[2]);
    EXPECT_EQ(2, pi[3]);
}

TEST(PermutationTest, RankAfterInsertion)
{
    permutation pi;
    construct_permutation(pi);
    EXPECT_EQ(3, pi.rank(0));
    EXPECT_EQ(4, pi.rank(1));
    EXPECT_EQ(1, pi.rank(2));
    EXPECT_EQ(2, pi.rank(3));
    EXPECT_EQ(5, pi.rank(4));
    EXPECT_EQ(0, pi.rank(5));
}

TEST(PermutationTest, RankAfterErasure)
{
    permutation pi;
    construct_permutation(pi);
    pi.erase(1);
    pi.erase(3);
    EXPECT_EQ(2, pi.rank(0));
    EXPECT_EQ(1, pi.rank(1));
    EXPECT_EQ(3, pi.rank(2));
    EXPECT_EQ(0, pi.rank(3));
}

TEST(PermutationTest, Move)
{
    permutation pi;
    construct_permutation(pi);
    pi.move(3, 1);

    EXPECT_EQ(5, pi[0]);
    EXPECT_EQ(0, pi[1]);
    EXPECT_EQ(2, pi[2]);
    EXPECT_EQ(3, pi[3]);
    EXPECT_EQ(1, pi[4]);
    EXPECT_EQ(4, pi[5]);

    EXPECT_EQ(1, pi.rank(0));
    EXPECT_EQ(4, pi.rank(1));
    EXPECT_EQ(2, pi.rank(2));
    EXPECT_EQ(3, pi.rank(3));
    EXPECT_EQ(5, pi.rank(4));
    EXPECT_EQ(0, pi.rank(5));
}
