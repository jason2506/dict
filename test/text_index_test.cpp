/************************************************
 *  text_index_test.cpp
 *  DESA
 *
 *  Copyright (c) 2015-2016, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <sstream>
#include <vector>
#include <initializer_list>

#include <gtest/gtest.h>

#include <desa/text_index.hpp>
#include <desa/with_csa.hpp>
#include <desa/with_lcp.hpp>

using text_index = desa::text_index<
    desa::with_csa,
    desa::with_lcp<>::policy
>;

// NOLINTNEXTLINE(runtime/references)
void insert(text_index &ti, std::initializer_list<text_index::term_type> seq) {
    ti.insert(seq);
}

void test(text_index const &ti,
          text_index::size_type num_seqs,
          text_index::size_type num_terms,
          std::vector<text_index::term_type> f,
          std::vector<text_index::term_type> bwt,
          std::vector<text_index::size_type> psi,
          std::vector<text_index::size_type> lf,
          std::vector<text_index::value_type> sa,
          std::vector<text_index::size_type> isa,
          std::vector<text_index::term_type> terms,
          std::vector<text_index::size_type> lcpa) {
    EXPECT_EQ(num_seqs, ti.num_seqs());
    EXPECT_EQ(num_terms, ti.num_terms());

    for (text_index::size_type i = 0; i < num_terms; i++) {
        std::ostringstream ss;
        ss << "i = " << i;
        SCOPED_TRACE(ss.str());

        EXPECT_EQ(f[i],     ti.f(i));
        EXPECT_EQ(bwt[i],   ti.bwt(i));
        EXPECT_EQ(psi[i],   ti.psi(i));
        EXPECT_EQ(lf[i],    ti.lf(i));
        EXPECT_EQ(sa[i],    ti.at(i));
        EXPECT_EQ(isa[i],   ti.rank(i));
        EXPECT_EQ(terms[i], ti.term(i));
        EXPECT_EQ(lcpa[i],  ti.lcp(i));
    }
}

TEST(SuffixArrayTest, EmptyArray) {
    text_index ti;
    test(ti, 0, 0,
        {},   // f
        {},   // bwt
        {},   // psi
        {},   // lf
        {},   // sa
        {},   // isa
        {},   // terms
        {});  // lcpa
}

TEST(SuffixArrayTest, InsertSingleValueIntoEmptyIndex) {
    text_index ti;
    insert(ti, {5});

    test(ti, 1, 2,
        {0, 5},   // f
        {5, 0},   // bwt
        {1, 0},   // psi
        {1, 0},   // lf
        {1, 0},   // sa
        {1, 0},   // isa
        {5, 0},   // terms
        {0, 0});  // lcpa
}

TEST(SuffixArrayTest, InsertMultipleValuesIntoEmptyIndex) {
    text_index ti;
    insert(ti, {1, 3, 1, 3, 2, 1});

    test(ti, 1, 7,
        {0, 1, 1, 1, 2, 3, 3},   // f
        {1, 2, 0, 3, 3, 1, 1},   // bwt
        {2, 0, 5, 6, 1, 3, 4},   // psi
        {1, 4, 0, 5, 6, 2, 3},   // lf
        {6, 5, 0, 2, 4, 1, 3},   // sa
        {2, 5, 3, 6, 4, 1, 0},   // isa
        {1, 3, 1, 3, 2, 1, 0},   // terms
        {0, 0, 1, 2, 0, 0, 1});  // lcpa
}

TEST(SuffixArrayTest, InsertMultipleValuesIntoNonEmptyIndex) {
    text_index ti;
    insert(ti, {1, 3, 1, 3, 2, 1});
    insert(ti, {1, 3, 2});

    test(ti, 2, 11,
        {0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3},   // f
        {1, 2, 2, 0, 0, 3, 3, 3, 1, 1, 1},   // bwt
        {4, 3, 0, 8, 9, 10, 1, 2, 5, 6, 7},  // psi
        {2, 6, 7, 1, 0, 8, 9, 10, 3, 4, 5},  // lf
        {10, 3, 9, 4, 0, 6, 2, 8, 5, 1, 7},  // sa
        {4, 9, 6, 1, 3, 8, 5, 10, 7, 2, 0},  // isa
        {1, 3, 2, 0, 1, 3, 1, 3, 2, 1, 0},   // terms
        {0, 0, 0, 1, 2, 3, 0, 1, 0, 1, 2});  // lcpa
}

TEST(SuffixArrayTest, InsertMultipleSequences) {
    text_index ti;
    insert(ti, {2, 1, 3});
    insert(ti, {2, 1});
    insert(ti, {1, 3, 2});

    test(ti, 3, 11,
        {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3},   // f
        {3, 2, 1, 2, 2, 0, 3, 0, 0, 1, 1},   // bwt
        {5, 7, 8, 2, 9, 10, 1, 3, 4, 0, 6},  // psi
        {9, 6, 3, 7, 8, 0, 10, 1, 2, 4, 5},  // lf
        {10, 3, 6, 5, 8, 0, 2, 4, 7, 9, 1},  // sa
        {5, 10, 6, 1, 7, 3, 2, 8, 4, 9, 0},  // isa
        {1, 3, 2, 0, 2, 1, 0, 2, 1, 3, 0},   // terms
        {0, 0, 0, 0, 1, 2, 0, 1, 2, 0, 1});  // lcpa
}
