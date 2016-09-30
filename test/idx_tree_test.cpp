/************************************************
 *  idx_tree_test.cpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <gtest/gtest.h>

#include "../src/idx_tree.hpp"

using desa::impl::idx_tree;

void construct_tree(idx_tree &tree, idx_tree::iterator its[9])
{
    its[5] = tree.insert(tree.end(), 5);    // 5
    its[3] = tree.insert(its[5], 3);        // 3 5
    its[7] = tree.insert(tree.end(), 7);    // 3 5 7
    its[2] = tree.insert(its[3], 2);        // 2 3 5 7
    its[6] = tree.insert(its[7], 6);        // 2 3 5 6 7
    its[4] = tree.insert(its[5], 4);        // 2 3 4 5 6 7
    its[8] = tree.insert(tree.end(), 8);    // 2 3 4 5 6 7 8
    its[1] = tree.insert(its[2], 1);        // 1 2 3 4 5 6 7 8
    its[0] = tree.insert(its[1], 0);        // 0 1 2 3 4 5 6 7 8
}

TEST(LcpArrayTest, EmptyArray)
{
    idx_tree tree;
    EXPECT_EQ(0, tree.size());
    EXPECT_EQ(tree.end(), tree.begin());
}

TEST(LcpArrayTest, InsertSingleValue)
{
    idx_tree tree;
    auto it = tree.insert(tree.begin(), 255);

    EXPECT_EQ(1, tree.size());

    EXPECT_EQ(it, --tree.end());
    EXPECT_EQ(it, tree.begin());
    EXPECT_EQ(it, tree.find(0));

    EXPECT_EQ(255, *it);
    EXPECT_EQ(255, tree.at(0));
}

TEST(LcpArrayTest, InsertMultipleValues)
{
    idx_tree tree;
    idx_tree::iterator its[9];
    construct_tree(tree, its);

    EXPECT_EQ(9, tree.size());

    auto it = tree.begin();
    for (::std::size_t i = 0; i < 9; ++i, ++it)
    {
        EXPECT_EQ(it, tree.find(i));
        EXPECT_EQ(i, tree[i]);
        EXPECT_EQ(i, *it);
    }

    EXPECT_EQ(tree.end(), it);
}

TEST(LcpArrayTest, EraseValues)
{
    idx_tree tree;
    idx_tree::iterator its[9];
    construct_tree(tree, its);

    tree.erase(tree.find(3)); // 0 1 2 4 5 6 7 8
    EXPECT_EQ(8, tree.size());
    EXPECT_EQ(4, tree[3]);

    tree.erase(tree.find(5)); // 0 1 2 4 5 7 8
    EXPECT_EQ(7, tree.size());
    EXPECT_EQ(7, tree[5]);
}
