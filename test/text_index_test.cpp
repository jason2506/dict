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
#include <desa/chained_updating_policy.hpp>
#include <desa/updating_lcp_policy.hpp>
#include <desa/updating_csa_policy.hpp>

using suffix_array = ::desa::text_index
<
    ::desa::chained_updating_policy
    <
        ::desa::updating_csa_policy,
        ::desa::updating_lcp_policy
    >::policy
>;

void insert(suffix_array &sa, ::std::vector<suffix_array::term_type> seq)
{
    sa.insert(seq.crbegin(), seq.crend());
}

TEST(SuffixArrayTest, EmptyArray)
{
    suffix_array sa;
    EXPECT_EQ(0, sa.size());
}

TEST(SuffixArrayTest, InsertSingleValueIntoEmptyArray)
{
    suffix_array sa;
    insert(sa, {5});

    EXPECT_EQ(2, sa.size());

    EXPECT_EQ(1, sa.lf(0));
    EXPECT_EQ(0, sa.lf(1));

    EXPECT_EQ(1, sa.psi(0));
    EXPECT_EQ(0, sa.psi(1));

    EXPECT_EQ(1, sa[0]);
    EXPECT_EQ(0, sa[1]);

    EXPECT_EQ(1, sa.rank(0));
    EXPECT_EQ(0, sa.rank(1));

    EXPECT_EQ(5, sa.bwt(0));
    EXPECT_EQ(0, sa.bwt(1));

    EXPECT_EQ(5, sa.term(0));
    EXPECT_EQ(0, sa.term(1));

    EXPECT_EQ(0, sa.lcp(0));
    EXPECT_EQ(0, sa.lcp(1));
}

TEST(SuffixArrayTest, InsertMultipleValuesIntoEmptyArray)
{
    suffix_array sa;
    insert(sa, {1, 3, 1, 3, 2, 1});

    EXPECT_EQ(7, sa.size());

    EXPECT_EQ(1, sa.lf(0));
    EXPECT_EQ(4, sa.lf(1));
    EXPECT_EQ(0, sa.lf(2));
    EXPECT_EQ(5, sa.lf(3));
    EXPECT_EQ(6, sa.lf(4));
    EXPECT_EQ(2, sa.lf(5));
    EXPECT_EQ(3, sa.lf(6));

    EXPECT_EQ(2, sa.psi(0));
    EXPECT_EQ(0, sa.psi(1));
    EXPECT_EQ(5, sa.psi(2));
    EXPECT_EQ(6, sa.psi(3));
    EXPECT_EQ(1, sa.psi(4));
    EXPECT_EQ(3, sa.psi(5));
    EXPECT_EQ(4, sa.psi(6));

    EXPECT_EQ(6, sa[0]);
    EXPECT_EQ(5, sa[1]);
    EXPECT_EQ(0, sa[2]);
    EXPECT_EQ(2, sa[3]);
    EXPECT_EQ(4, sa[4]);
    EXPECT_EQ(1, sa[5]);
    EXPECT_EQ(3, sa[6]);

    EXPECT_EQ(2, sa.rank(0));
    EXPECT_EQ(5, sa.rank(1));
    EXPECT_EQ(3, sa.rank(2));
    EXPECT_EQ(6, sa.rank(3));
    EXPECT_EQ(4, sa.rank(4));
    EXPECT_EQ(1, sa.rank(5));
    EXPECT_EQ(0, sa.rank(6));

    EXPECT_EQ(1, sa.bwt(0));
    EXPECT_EQ(2, sa.bwt(1));
    EXPECT_EQ(0, sa.bwt(2));
    EXPECT_EQ(3, sa.bwt(3));
    EXPECT_EQ(3, sa.bwt(4));
    EXPECT_EQ(1, sa.bwt(5));
    EXPECT_EQ(1, sa.bwt(6));

    EXPECT_EQ(1, sa.term(0));
    EXPECT_EQ(3, sa.term(1));
    EXPECT_EQ(1, sa.term(2));
    EXPECT_EQ(3, sa.term(3));
    EXPECT_EQ(2, sa.term(4));
    EXPECT_EQ(1, sa.term(5));
    EXPECT_EQ(0, sa.term(6));

    EXPECT_EQ(0, sa.lcp(0));
    EXPECT_EQ(0, sa.lcp(1));
    EXPECT_EQ(1, sa.lcp(2));
    EXPECT_EQ(2, sa.lcp(3));
    EXPECT_EQ(0, sa.lcp(4));
    EXPECT_EQ(0, sa.lcp(5));
    EXPECT_EQ(1, sa.lcp(6));
}

TEST(SuffixArrayTest, InsertMultipleValuesIntoNonEmptyArray)
{
    suffix_array sa;
    insert(sa, {1, 3, 1, 3, 2, 1});
    insert(sa, {1, 3, 2});

    EXPECT_EQ(11, sa.size());

    EXPECT_EQ(2, sa.lf(0));
    EXPECT_EQ(6, sa.lf(1));
    EXPECT_EQ(7, sa.lf(2));
    EXPECT_EQ(1, sa.lf(3));
    EXPECT_EQ(0, sa.lf(4));
    EXPECT_EQ(8, sa.lf(5));
    EXPECT_EQ(9, sa.lf(6));
    EXPECT_EQ(10, sa.lf(7));
    EXPECT_EQ(3, sa.lf(8));
    EXPECT_EQ(4, sa.lf(9));
    EXPECT_EQ(5, sa.lf(10));

    EXPECT_EQ(4, sa.psi(0));
    EXPECT_EQ(3, sa.psi(1));
    EXPECT_EQ(0, sa.psi(2));
    EXPECT_EQ(8, sa.psi(3));
    EXPECT_EQ(9, sa.psi(4));
    EXPECT_EQ(10, sa.psi(5));
    EXPECT_EQ(1, sa.psi(6));
    EXPECT_EQ(2, sa.psi(7));
    EXPECT_EQ(5, sa.psi(8));
    EXPECT_EQ(6, sa.psi(9));
    EXPECT_EQ(7, sa.psi(10));

    EXPECT_EQ(10, sa[0]);
    EXPECT_EQ(3, sa[1]);
    EXPECT_EQ(9, sa[2]);
    EXPECT_EQ(4, sa[3]);
    EXPECT_EQ(0, sa[4]);
    EXPECT_EQ(6, sa[5]);
    EXPECT_EQ(2, sa[6]);
    EXPECT_EQ(8, sa[7]);
    EXPECT_EQ(5, sa[8]);
    EXPECT_EQ(1, sa[9]);
    EXPECT_EQ(7, sa[10]);

    EXPECT_EQ(4, sa.rank(0));
    EXPECT_EQ(9, sa.rank(1));
    EXPECT_EQ(6, sa.rank(2));
    EXPECT_EQ(1, sa.rank(3));
    EXPECT_EQ(3, sa.rank(4));
    EXPECT_EQ(8, sa.rank(5));
    EXPECT_EQ(5, sa.rank(6));
    EXPECT_EQ(10, sa.rank(7));
    EXPECT_EQ(7, sa.rank(8));
    EXPECT_EQ(2, sa.rank(9));
    EXPECT_EQ(0, sa.rank(10));

    EXPECT_EQ(1, sa.bwt(0));
    EXPECT_EQ(2, sa.bwt(1));
    EXPECT_EQ(2, sa.bwt(2));
    EXPECT_EQ(0, sa.bwt(3));
    EXPECT_EQ(0, sa.bwt(4));
    EXPECT_EQ(3, sa.bwt(5));
    EXPECT_EQ(3, sa.bwt(6));
    EXPECT_EQ(3, sa.bwt(7));
    EXPECT_EQ(1, sa.bwt(8));
    EXPECT_EQ(1, sa.bwt(9));
    EXPECT_EQ(1, sa.bwt(10));

    EXPECT_EQ(1, sa.term(0));
    EXPECT_EQ(3, sa.term(1));
    EXPECT_EQ(2, sa.term(2));
    EXPECT_EQ(0, sa.term(3));
    EXPECT_EQ(1, sa.term(4));
    EXPECT_EQ(3, sa.term(5));
    EXPECT_EQ(1, sa.term(6));
    EXPECT_EQ(3, sa.term(7));
    EXPECT_EQ(2, sa.term(8));
    EXPECT_EQ(1, sa.term(9));
    EXPECT_EQ(0, sa.term(10));

    EXPECT_EQ(0, sa.lcp(0));
    EXPECT_EQ(0, sa.lcp(1));
    EXPECT_EQ(0, sa.lcp(2));
    EXPECT_EQ(1, sa.lcp(3));
    EXPECT_EQ(2, sa.lcp(4));
    EXPECT_EQ(3, sa.lcp(5));
    EXPECT_EQ(0, sa.lcp(6));
    EXPECT_EQ(1, sa.lcp(7));
    EXPECT_EQ(0, sa.lcp(8));
    EXPECT_EQ(1, sa.lcp(9));
    EXPECT_EQ(2, sa.lcp(10));
}

TEST(SuffixArrayTest, InsertMultipleSequences)
{
    suffix_array sa;
    insert(sa, {3});
    insert(sa, {1});
    insert(sa, {2});

    EXPECT_EQ(6, sa.size());

    EXPECT_EQ(5, sa.lf(0));
    EXPECT_EQ(4, sa.lf(1));
    EXPECT_EQ(3, sa.lf(2));
    EXPECT_EQ(1, sa.lf(3));
    EXPECT_EQ(0, sa.lf(4));
    EXPECT_EQ(2, sa.lf(5));

    EXPECT_EQ(4, sa.psi(0));
    EXPECT_EQ(3, sa.psi(1));
    EXPECT_EQ(5, sa.psi(2));
    EXPECT_EQ(2, sa.psi(3));
    EXPECT_EQ(1, sa.psi(4));
    EXPECT_EQ(0, sa.psi(5));

    EXPECT_EQ(5, sa[0]);
    EXPECT_EQ(1, sa[1]);
    EXPECT_EQ(3, sa[2]);
    EXPECT_EQ(2, sa[3]);
    EXPECT_EQ(0, sa[4]);
    EXPECT_EQ(4, sa[5]);

    EXPECT_EQ(4, sa.rank(0));
    EXPECT_EQ(1, sa.rank(1));
    EXPECT_EQ(3, sa.rank(2));
    EXPECT_EQ(2, sa.rank(3));
    EXPECT_EQ(5, sa.rank(4));
    EXPECT_EQ(0, sa.rank(5));

    EXPECT_EQ(3, sa.bwt(0));
    EXPECT_EQ(2, sa.bwt(1));
    EXPECT_EQ(1, sa.bwt(2));
    EXPECT_EQ(0, sa.bwt(3));
    EXPECT_EQ(0, sa.bwt(4));
    EXPECT_EQ(0, sa.bwt(5));

    EXPECT_EQ(2, sa.term(0));
    EXPECT_EQ(0, sa.term(1));
    EXPECT_EQ(1, sa.term(2));
    EXPECT_EQ(0, sa.term(3));
    EXPECT_EQ(3, sa.term(4));
    EXPECT_EQ(0, sa.term(5));

    EXPECT_EQ(0, sa.lcp(0));
    EXPECT_EQ(0, sa.lcp(1));
    EXPECT_EQ(0, sa.lcp(2));
    EXPECT_EQ(0, sa.lcp(3));
    EXPECT_EQ(0, sa.lcp(4));
    EXPECT_EQ(0, sa.lcp(5));
}
