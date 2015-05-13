/************************************************
 *  partial_sum_test.cpp
 *  ESA++
 *
 *  Copyright (c) 2014-2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <stdexcept>

#include <gtest/gtest.h>

#include "../src/partial_sum.hpp"

using esapp::impl::partial_sum;

typedef partial_sum<char, ::std::size_t> ps_t;

void construct_partial_sum_with_more_counts(ps_t &ps)
{
    ps.increase('B', 7);
    ps.increase('A', 9);
    ps.increase('D', 3);
}

void construct_partial_sum_with_large_amount_of_counts(ps_t &ps)
{
    ps.increase('A', 1);    ps.increase('I', 9);    ps.increase('S', 19);
    ps.increase('P', 16);   ps.increase('E', 5);    ps.increase('F', 6);
    ps.increase('J', 10);   ps.increase('M', 13);   ps.increase('R', 18);
    ps.increase('N', 14);   ps.increase('L', 12);   ps.increase('Q', 17);
    ps.increase('D', 4);    ps.increase('Y', 25);   ps.increase('O', 15);
    ps.increase('K', 11);   ps.increase('B', 2);    ps.increase('C', 3);
    ps.increase('X', 24);   ps.increase('G', 7);    ps.increase('H', 8);
    ps.increase('V', 22);   ps.increase('W', 23);   ps.increase('Z', 26);
    ps.increase('T', 20);   ps.increase('U', 21);
}

TEST(PartialSumTest, PartialSumWithoutKey)
{
    ps_t ps;
    EXPECT_EQ(ps.sum(), 0);
}

TEST(PartialSumTest, IncreaseSingleCount)
{
    ps_t ps;
    ps.increase('B', 5);

    EXPECT_EQ(ps.sum('B'), 5);
    EXPECT_EQ(ps.sum(), 5);
}

TEST(PartialSumTest, IncreaseSingleCountTwice)
{
    ps_t ps;
    ps.increase('B', 5);
    ps.increase('B', 8);

    EXPECT_EQ(ps.sum('B'), 13);
    EXPECT_EQ(ps.sum(), 13);
}

TEST(PartialSumTest, DecreaseSingleCount)
{
    ps_t ps;
    ps.increase('B', 5);
    ps.decrease('B', 2);

    EXPECT_EQ(ps.sum('B'), 3);
    EXPECT_EQ(ps.sum(), 3);
}

TEST(PartialSumTest, SumWithNonExistedKey)
{
    ps_t ps;
    EXPECT_EQ(ps.sum('B'), 0);

    ps.increase('B', 5);

    EXPECT_EQ(ps.sum('A'), 0);
    EXPECT_EQ(ps.sum('C'), 5);
}

TEST(PartialSumTest, IncreaseMoreCounts)
{
    ps_t ps;
    construct_partial_sum_with_more_counts(ps);

    EXPECT_EQ(ps.sum('A'), 9);
    EXPECT_EQ(ps.sum('B'), 16);
    EXPECT_EQ(ps.sum('C'), 16);
    EXPECT_EQ(ps.sum('D'), 19);
    EXPECT_EQ(ps.sum('E'), 19);
    EXPECT_EQ(ps.sum(), 19);
}

TEST(PartialSumTest, DecreaseMoreCounts)
{
    ps_t ps;
    construct_partial_sum_with_more_counts(ps);

    ps.decrease('A', 1);
    ps.decrease('D', 2);

    EXPECT_EQ(ps.sum('A'), 8);
    EXPECT_EQ(ps.sum('B'), 15);
    EXPECT_EQ(ps.sum('C'), 15);
    EXPECT_EQ(ps.sum('D'), 16);
    EXPECT_EQ(ps.sum('E'), 16);
    EXPECT_EQ(ps.sum(), 16);
}

TEST(PartialSumTest, IncreaseLargeAmountOfCounts)
{
    ps_t ps;
    construct_partial_sum_with_large_amount_of_counts(ps);

    EXPECT_EQ(ps.sum('A'), 1);      EXPECT_EQ(ps.sum('B'), 3);
    EXPECT_EQ(ps.sum('C'), 6);      EXPECT_EQ(ps.sum('D'), 10);
    EXPECT_EQ(ps.sum('E'), 15);     EXPECT_EQ(ps.sum('F'), 21);
    EXPECT_EQ(ps.sum('G'), 28);     EXPECT_EQ(ps.sum('H'), 36);
    EXPECT_EQ(ps.sum('I'), 45);     EXPECT_EQ(ps.sum('J'), 55);
    EXPECT_EQ(ps.sum('K'), 66);     EXPECT_EQ(ps.sum('L'), 78);
    EXPECT_EQ(ps.sum('M'), 91);     EXPECT_EQ(ps.sum('N'), 105);
    EXPECT_EQ(ps.sum('O'), 120);    EXPECT_EQ(ps.sum('P'), 136);
    EXPECT_EQ(ps.sum('Q'), 153);    EXPECT_EQ(ps.sum('R'), 171);
    EXPECT_EQ(ps.sum('S'), 190);    EXPECT_EQ(ps.sum('T'), 210);
    EXPECT_EQ(ps.sum('U'), 231);    EXPECT_EQ(ps.sum('V'), 253);
    EXPECT_EQ(ps.sum('W'), 276);    EXPECT_EQ(ps.sum('X'), 300);
    EXPECT_EQ(ps.sum('Y'), 325);    EXPECT_EQ(ps.sum('Z'), 351);
    EXPECT_EQ(ps.sum(), 351);
}

TEST(PartialSumTest, SearchOnLargeAmountOfCounts)
{
    ps_t ps;
    construct_partial_sum_with_large_amount_of_counts(ps);

    EXPECT_EQ(ps.search(0), 'A');
    EXPECT_EQ(ps.search(32), 'H');
    EXPECT_EQ(ps.search(64), 'K');
    EXPECT_EQ(ps.search(128), 'P');
    EXPECT_EQ(ps.search(256), 'W');
    EXPECT_EQ(ps.search(300), 'X');
    EXPECT_EQ(ps.search(301), 'Y');
    EXPECT_EQ(ps.search(351), 'Z');
    EXPECT_THROW(ps.search(352), ::std::invalid_argument);
}
