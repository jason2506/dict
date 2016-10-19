/************************************************
 *  partial_sum_test.cpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <stdexcept>

#include <gtest/gtest.h>

#include <desa/internal/partial_sum.hpp>

using ::desa::internal::partial_sum;

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
    EXPECT_EQ(0, ps.sum());
}

TEST(PartialSumTest, IncreaseSingleCount)
{
    ps_t ps;
    ps.increase('B', 5);

    EXPECT_EQ(5, ps.sum('B'));
    EXPECT_EQ(5, ps.sum());
}

TEST(PartialSumTest, IncreaseSingleCountTwice)
{
    ps_t ps;
    ps.increase('B', 5);
    ps.increase('B', 8);

    EXPECT_EQ(13, ps.sum('B'));
    EXPECT_EQ(13, ps.sum());
}

TEST(PartialSumTest, DecreaseSingleCount)
{
    ps_t ps;
    ps.increase('B', 5);
    ps.decrease('B', 2);

    EXPECT_EQ(3, ps.sum('B'));
    EXPECT_EQ(3, ps.sum());
}

TEST(PartialSumTest, SumWithNonExistedKey)
{
    ps_t ps;
    EXPECT_EQ(0, ps.sum('B'));

    ps.increase('B', 5);

    EXPECT_EQ(0, ps.sum('A'));
    EXPECT_EQ(5, ps.sum('C'));
}

TEST(PartialSumTest, IncreaseMoreCounts)
{
    ps_t ps;
    construct_partial_sum_with_more_counts(ps);

    EXPECT_EQ(9, ps.sum('A'));
    EXPECT_EQ(16, ps.sum('B'));
    EXPECT_EQ(16, ps.sum('C'));
    EXPECT_EQ(19, ps.sum('D'));
    EXPECT_EQ(19, ps.sum('E'));
    EXPECT_EQ(19, ps.sum());
}

TEST(PartialSumTest, DecreaseMoreCounts)
{
    ps_t ps;
    construct_partial_sum_with_more_counts(ps);

    ps.decrease('A', 1);
    ps.decrease('D', 2);

    EXPECT_EQ(8, ps.sum('A'));
    EXPECT_EQ(15, ps.sum('B'));
    EXPECT_EQ(15, ps.sum('C'));
    EXPECT_EQ(16, ps.sum('D'));
    EXPECT_EQ(16, ps.sum('E'));
    EXPECT_EQ(16, ps.sum());
}

TEST(PartialSumTest, IncreaseLargeAmountOfCounts)
{
    ps_t ps;
    construct_partial_sum_with_large_amount_of_counts(ps);

    EXPECT_EQ(1, ps.sum('A'));      EXPECT_EQ(3, ps.sum('B'));
    EXPECT_EQ(6, ps.sum('C'));      EXPECT_EQ(10, ps.sum('D'));
    EXPECT_EQ(15, ps.sum('E'));     EXPECT_EQ(21, ps.sum('F'));
    EXPECT_EQ(28, ps.sum('G'));     EXPECT_EQ(36, ps.sum('H'));
    EXPECT_EQ(45, ps.sum('I'));     EXPECT_EQ(55, ps.sum('J'));
    EXPECT_EQ(66, ps.sum('K'));     EXPECT_EQ(78, ps.sum('L'));
    EXPECT_EQ(91, ps.sum('M'));     EXPECT_EQ(105, ps.sum('N'));
    EXPECT_EQ(120, ps.sum('O'));    EXPECT_EQ(136, ps.sum('P'));
    EXPECT_EQ(153, ps.sum('Q'));    EXPECT_EQ(171, ps.sum('R'));
    EXPECT_EQ(190, ps.sum('S'));    EXPECT_EQ(210, ps.sum('T'));
    EXPECT_EQ(231, ps.sum('U'));    EXPECT_EQ(253, ps.sum('V'));
    EXPECT_EQ(276, ps.sum('W'));    EXPECT_EQ(300, ps.sum('X'));
    EXPECT_EQ(325, ps.sum('Y'));    EXPECT_EQ(351, ps.sum('Z'));
    EXPECT_EQ(351, ps.sum());
}

TEST(PartialSumTest, SearchOnLargeAmountOfCounts)
{
    ps_t ps;
    construct_partial_sum_with_large_amount_of_counts(ps);

    EXPECT_EQ('A', ps.search(0));
    EXPECT_EQ('H', ps.search(32));
    EXPECT_EQ('K', ps.search(64));
    EXPECT_EQ('P', ps.search(128));
    EXPECT_EQ('W', ps.search(256));
    EXPECT_EQ('X', ps.search(300));
    EXPECT_EQ('Y', ps.search(301));
    EXPECT_EQ('Z', ps.search(351));
    EXPECT_THROW(ps.search(352), ::std::invalid_argument);
}
