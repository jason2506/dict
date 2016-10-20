/************************************************
 *  text_index_test.cpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <vector>

#include <gtest/gtest.h>

#include <desa/text_index.hpp>
#include <desa/with_csa.hpp>
#include <desa/with_lcp.hpp>

using text_index = ::desa::text_index
    <
        ::desa::with_csa,
        ::desa::with_lcp<>::policy
    >;

void insert(text_index &ti, ::std::vector<text_index::term_type> seq)
{
    ti.insert(seq.crbegin(), seq.crend());
}

TEST(SuffixArrayTest, EmptyArray)
{
    text_index ti;
    EXPECT_EQ(0, ti.size());
}

TEST(SuffixArrayTest, InsertSingleValueIntoEmptyArray)
{
    text_index ti;
    insert(ti, {5});

    EXPECT_EQ(2, ti.size());

    EXPECT_EQ(1, ti.lf(0));
    EXPECT_EQ(0, ti.lf(1));

    EXPECT_EQ(1, ti.psi(0));
    EXPECT_EQ(0, ti.psi(1));

    EXPECT_EQ(1, ti[0]);
    EXPECT_EQ(0, ti[1]);

    EXPECT_EQ(1, ti.rank(0));
    EXPECT_EQ(0, ti.rank(1));

    EXPECT_EQ(5, ti.bwt(0));
    EXPECT_EQ(0, ti.bwt(1));

    EXPECT_EQ(5, ti.term(0));
    EXPECT_EQ(0, ti.term(1));

    EXPECT_EQ(0, ti.lcp(0));
    EXPECT_EQ(0, ti.lcp(1));
}

TEST(SuffixArrayTest, InsertMultipleValuesIntoEmptyArray)
{
    text_index ti;
    insert(ti, {1, 3, 1, 3, 2, 1});

    EXPECT_EQ(7, ti.size());

    EXPECT_EQ(1, ti.lf(0));
    EXPECT_EQ(4, ti.lf(1));
    EXPECT_EQ(0, ti.lf(2));
    EXPECT_EQ(5, ti.lf(3));
    EXPECT_EQ(6, ti.lf(4));
    EXPECT_EQ(2, ti.lf(5));
    EXPECT_EQ(3, ti.lf(6));

    EXPECT_EQ(2, ti.psi(0));
    EXPECT_EQ(0, ti.psi(1));
    EXPECT_EQ(5, ti.psi(2));
    EXPECT_EQ(6, ti.psi(3));
    EXPECT_EQ(1, ti.psi(4));
    EXPECT_EQ(3, ti.psi(5));
    EXPECT_EQ(4, ti.psi(6));

    EXPECT_EQ(6, ti[0]);
    EXPECT_EQ(5, ti[1]);
    EXPECT_EQ(0, ti[2]);
    EXPECT_EQ(2, ti[3]);
    EXPECT_EQ(4, ti[4]);
    EXPECT_EQ(1, ti[5]);
    EXPECT_EQ(3, ti[6]);

    EXPECT_EQ(2, ti.rank(0));
    EXPECT_EQ(5, ti.rank(1));
    EXPECT_EQ(3, ti.rank(2));
    EXPECT_EQ(6, ti.rank(3));
    EXPECT_EQ(4, ti.rank(4));
    EXPECT_EQ(1, ti.rank(5));
    EXPECT_EQ(0, ti.rank(6));

    EXPECT_EQ(1, ti.bwt(0));
    EXPECT_EQ(2, ti.bwt(1));
    EXPECT_EQ(0, ti.bwt(2));
    EXPECT_EQ(3, ti.bwt(3));
    EXPECT_EQ(3, ti.bwt(4));
    EXPECT_EQ(1, ti.bwt(5));
    EXPECT_EQ(1, ti.bwt(6));

    EXPECT_EQ(1, ti.term(0));
    EXPECT_EQ(3, ti.term(1));
    EXPECT_EQ(1, ti.term(2));
    EXPECT_EQ(3, ti.term(3));
    EXPECT_EQ(2, ti.term(4));
    EXPECT_EQ(1, ti.term(5));
    EXPECT_EQ(0, ti.term(6));

    EXPECT_EQ(0, ti.lcp(0));
    EXPECT_EQ(0, ti.lcp(1));
    EXPECT_EQ(1, ti.lcp(2));
    EXPECT_EQ(2, ti.lcp(3));
    EXPECT_EQ(0, ti.lcp(4));
    EXPECT_EQ(0, ti.lcp(5));
    EXPECT_EQ(1, ti.lcp(6));
}

TEST(SuffixArrayTest, InsertMultipleValuesIntoNonEmptyArray)
{
    text_index ti;
    insert(ti, {1, 3, 1, 3, 2, 1});
    insert(ti, {1, 3, 2});

    EXPECT_EQ(11, ti.size());

    EXPECT_EQ(2, ti.lf(0));
    EXPECT_EQ(6, ti.lf(1));
    EXPECT_EQ(7, ti.lf(2));
    EXPECT_EQ(1, ti.lf(3));
    EXPECT_EQ(0, ti.lf(4));
    EXPECT_EQ(8, ti.lf(5));
    EXPECT_EQ(9, ti.lf(6));
    EXPECT_EQ(10, ti.lf(7));
    EXPECT_EQ(3, ti.lf(8));
    EXPECT_EQ(4, ti.lf(9));
    EXPECT_EQ(5, ti.lf(10));

    EXPECT_EQ(4, ti.psi(0));
    EXPECT_EQ(3, ti.psi(1));
    EXPECT_EQ(0, ti.psi(2));
    EXPECT_EQ(8, ti.psi(3));
    EXPECT_EQ(9, ti.psi(4));
    EXPECT_EQ(10, ti.psi(5));
    EXPECT_EQ(1, ti.psi(6));
    EXPECT_EQ(2, ti.psi(7));
    EXPECT_EQ(5, ti.psi(8));
    EXPECT_EQ(6, ti.psi(9));
    EXPECT_EQ(7, ti.psi(10));

    EXPECT_EQ(10, ti[0]);
    EXPECT_EQ(3, ti[1]);
    EXPECT_EQ(9, ti[2]);
    EXPECT_EQ(4, ti[3]);
    EXPECT_EQ(0, ti[4]);
    EXPECT_EQ(6, ti[5]);
    EXPECT_EQ(2, ti[6]);
    EXPECT_EQ(8, ti[7]);
    EXPECT_EQ(5, ti[8]);
    EXPECT_EQ(1, ti[9]);
    EXPECT_EQ(7, ti[10]);

    EXPECT_EQ(4, ti.rank(0));
    EXPECT_EQ(9, ti.rank(1));
    EXPECT_EQ(6, ti.rank(2));
    EXPECT_EQ(1, ti.rank(3));
    EXPECT_EQ(3, ti.rank(4));
    EXPECT_EQ(8, ti.rank(5));
    EXPECT_EQ(5, ti.rank(6));
    EXPECT_EQ(10, ti.rank(7));
    EXPECT_EQ(7, ti.rank(8));
    EXPECT_EQ(2, ti.rank(9));
    EXPECT_EQ(0, ti.rank(10));

    EXPECT_EQ(1, ti.bwt(0));
    EXPECT_EQ(2, ti.bwt(1));
    EXPECT_EQ(2, ti.bwt(2));
    EXPECT_EQ(0, ti.bwt(3));
    EXPECT_EQ(0, ti.bwt(4));
    EXPECT_EQ(3, ti.bwt(5));
    EXPECT_EQ(3, ti.bwt(6));
    EXPECT_EQ(3, ti.bwt(7));
    EXPECT_EQ(1, ti.bwt(8));
    EXPECT_EQ(1, ti.bwt(9));
    EXPECT_EQ(1, ti.bwt(10));

    EXPECT_EQ(1, ti.term(0));
    EXPECT_EQ(3, ti.term(1));
    EXPECT_EQ(2, ti.term(2));
    EXPECT_EQ(0, ti.term(3));
    EXPECT_EQ(1, ti.term(4));
    EXPECT_EQ(3, ti.term(5));
    EXPECT_EQ(1, ti.term(6));
    EXPECT_EQ(3, ti.term(7));
    EXPECT_EQ(2, ti.term(8));
    EXPECT_EQ(1, ti.term(9));
    EXPECT_EQ(0, ti.term(10));

    EXPECT_EQ(0, ti.lcp(0));
    EXPECT_EQ(0, ti.lcp(1));
    EXPECT_EQ(0, ti.lcp(2));
    EXPECT_EQ(1, ti.lcp(3));
    EXPECT_EQ(2, ti.lcp(4));
    EXPECT_EQ(3, ti.lcp(5));
    EXPECT_EQ(0, ti.lcp(6));
    EXPECT_EQ(1, ti.lcp(7));
    EXPECT_EQ(0, ti.lcp(8));
    EXPECT_EQ(1, ti.lcp(9));
    EXPECT_EQ(2, ti.lcp(10));
}

TEST(SuffixArrayTest, InsertMultipleSequences)
{
    text_index ti;
    insert(ti, {3});
    insert(ti, {1});
    insert(ti, {2});

    EXPECT_EQ(6, ti.size());

    EXPECT_EQ(5, ti.lf(0));
    EXPECT_EQ(4, ti.lf(1));
    EXPECT_EQ(3, ti.lf(2));
    EXPECT_EQ(1, ti.lf(3));
    EXPECT_EQ(0, ti.lf(4));
    EXPECT_EQ(2, ti.lf(5));

    EXPECT_EQ(4, ti.psi(0));
    EXPECT_EQ(3, ti.psi(1));
    EXPECT_EQ(5, ti.psi(2));
    EXPECT_EQ(2, ti.psi(3));
    EXPECT_EQ(1, ti.psi(4));
    EXPECT_EQ(0, ti.psi(5));

    EXPECT_EQ(5, ti[0]);
    EXPECT_EQ(1, ti[1]);
    EXPECT_EQ(3, ti[2]);
    EXPECT_EQ(2, ti[3]);
    EXPECT_EQ(0, ti[4]);
    EXPECT_EQ(4, ti[5]);

    EXPECT_EQ(4, ti.rank(0));
    EXPECT_EQ(1, ti.rank(1));
    EXPECT_EQ(3, ti.rank(2));
    EXPECT_EQ(2, ti.rank(3));
    EXPECT_EQ(5, ti.rank(4));
    EXPECT_EQ(0, ti.rank(5));

    EXPECT_EQ(3, ti.bwt(0));
    EXPECT_EQ(2, ti.bwt(1));
    EXPECT_EQ(1, ti.bwt(2));
    EXPECT_EQ(0, ti.bwt(3));
    EXPECT_EQ(0, ti.bwt(4));
    EXPECT_EQ(0, ti.bwt(5));

    EXPECT_EQ(2, ti.term(0));
    EXPECT_EQ(0, ti.term(1));
    EXPECT_EQ(1, ti.term(2));
    EXPECT_EQ(0, ti.term(3));
    EXPECT_EQ(3, ti.term(4));
    EXPECT_EQ(0, ti.term(5));

    EXPECT_EQ(0, ti.lcp(0));
    EXPECT_EQ(0, ti.lcp(1));
    EXPECT_EQ(0, ti.lcp(2));
    EXPECT_EQ(0, ti.lcp(3));
    EXPECT_EQ(0, ti.lcp(4));
    EXPECT_EQ(0, ti.lcp(5));
}
