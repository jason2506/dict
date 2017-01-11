/************************************************
 *  rbtree_test.cpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <sstream>
#include <utility>

#include <gtest/gtest.h>

#include <dict/internal/rbtree.hpp>

// TODO(jason2506) there should be a better way for testing insertion/erasure operation

#define INSERT_CHECK(tree, in_it, out_it, i) { \
        std::ostringstream ss; \
        ss << "After inserting: " << i; \
        SCOPED_TRACE(ss.str()); \
        auto inserted_it = in_it; \
        auto new_it = tree.insert_before(inserted_it, i, noop); \
        out_it = new_it++; \
        EXPECT_EQ(inserted_it, new_it); \
        check_rbtree_property(tree); \
    }

#define ERASE_CHECK(tree, it) { \
        std::ostringstream ss; \
        ss << "After erasing: " << *it; \
        SCOPED_TRACE(ss.str()); \
        auto deleted_it = it++; \
        auto next_it = tree.erase(deleted_it, noop); \
        EXPECT_EQ(it, next_it); \
        check_rbtree_property(tree); \
    }

using dict::internal::rbtree;
using rbtree_node_ptr = decltype(std::declval<typename rbtree<int>::iterator>().get_node_ptr());
using rbtree_node_color = decltype(rbtree_node_ptr()->get_color());

auto noop = [](rbtree<int>::iterator){};

bool is_black_node(rbtree_node_ptr ptr, rbtree_node_color black) {
    // all leaves (null) are black
    return ptr == nullptr || ptr->get_color() == black;
}

void check_subtree_property(  // NOLINTNEXTLINE(runtime/references)
        rbtree_node_ptr ptr, rbtree_node_color black, std::size_t &num_black_nodes) {
    if (ptr == nullptr) {
        // all leaves (null) are black
        num_black_nodes = 1;
        return;
    }

    if (!is_black_node(ptr, black)) {
        // red node must have two black child nodes
        ASSERT_TRUE(is_black_node(ptr->get_left(), black));
        ASSERT_TRUE(is_black_node(ptr->get_right(), black));
    }

    // check the subtrees recursively
    std::size_t num_left_black_nodes, num_right_black_nodes;
    check_subtree_property(ptr->get_left(), black, num_left_black_nodes);
    if (::testing::Test::HasFatalFailure()) { return; }
    check_subtree_property(ptr->get_right(), black, num_right_black_nodes);
    if (::testing::Test::HasFatalFailure()) { return; }

    // every path from a node to any of its descendant null nodes contains the
    // same number of black nodes
    ASSERT_EQ(num_left_black_nodes, num_right_black_nodes);

    num_black_nodes = num_left_black_nodes + (is_black_node(ptr, black) ? 1 : 0);
}

// NOLINTNEXTLINE(runtime/references)
void check_rbtree_property(rbtree<int> &tree) {
    // NOTE: we assume that the tree satisfies all properties is correct
    auto root = tree.root().get_node_ptr();
    if (root == nullptr) { return; }

    auto black = root->get_color();  // root is black
    std::size_t num_black_nodes;
    check_subtree_property(root, black, num_black_nodes);
}

template <std::size_t N>  // NOLINTNEXTLINE(runtime/references)
void construct_tree(rbtree<int> &tree, rbtree<int>::iterator its[N]) {
    for (decltype(N) i = 0; i < N; ++i) {
        its[i] = tree.insert_before(tree.end(), i, noop);
    }
}

TEST(RBTreeTest, EmptyTree) {
    rbtree<int> tree;
    EXPECT_EQ(0, tree.size());
    EXPECT_EQ(tree.end(), tree.root());
    EXPECT_EQ(tree.end(), tree.begin());
}

TEST(RBTreeTest, InsertFirstNode) {
    rbtree<int> tree;
    auto it = tree.insert_before(tree.end(), 9, noop);
    EXPECT_EQ(1, tree.size());
    EXPECT_EQ(tree.root(), it);
    EXPECT_EQ(tree.begin(), it);
    EXPECT_EQ(--tree.end(), it);
    EXPECT_EQ(9, *it);
}

TEST(RBTreeTest, InsertAtBegin) {
    constexpr std::size_t N = 9;

    rbtree<int> tree;
    rbtree<int>::iterator it;
    for (auto i = N; i > 0; --i) {
        INSERT_CHECK(tree, it, it, i);
        EXPECT_EQ(tree.begin(), it);
    }

    EXPECT_EQ(N, tree.size());

    // check values stored in the tree
    it = tree.begin();
    for (::size_t i = 1; i <= N; ++i, ++it) {
        EXPECT_EQ(i, *it);
    }

    EXPECT_EQ(tree.end(), it);
}

TEST(RBTreeTest, InsertAtEnd) {
    constexpr std::size_t N = 9;

    rbtree<int> tree;
    rbtree<int>::iterator it;
    for (auto i = N; i > 0; --i) {
        INSERT_CHECK(tree, tree.end(), it, i);
        EXPECT_EQ(--tree.end(), it);
    }

    EXPECT_EQ(N, tree.size());

    // check values stored in the tree
    it = tree.begin();
    for (auto i = N; i > 0; --i, ++it) {
        EXPECT_EQ(i, *it);
    }

    EXPECT_EQ(tree.end(), it);
}

TEST(RBTreeTest, InsertInShuffleOrder) {
    rbtree<int> tree;

    rbtree<int>::iterator its[9];
    INSERT_CHECK(tree, tree.end(), its[5], 5);  // 5
    INSERT_CHECK(tree, its[5], its[3], 3);      // 3 5
    INSERT_CHECK(tree, tree.end(), its[7], 7);  // 3 5 7
    INSERT_CHECK(tree, its[3], its[2], 2);      // 2 3 5 7
    INSERT_CHECK(tree, its[7], its[6], 6);      // 2 3 5 6 7
    INSERT_CHECK(tree, its[5], its[4], 4);      // 2 3 4 5 6 7
    INSERT_CHECK(tree, tree.end(), its[8], 8);  // 2 3 4 5 6 7 8
    INSERT_CHECK(tree, its[2], its[1], 1);      // 1 2 3 4 5 6 7 8
    INSERT_CHECK(tree, its[1], its[0], 0);      // 0 1 2 3 4 5 6 7 8

    EXPECT_EQ(9, tree.size());

    // check values stored in the tree
    auto it = tree.begin();
    for (std::size_t i = 0; i < 9; ++i, ++it) {
        EXPECT_EQ(i, *it);
    }

    EXPECT_EQ(tree.end(), it);
}

TEST(RBTreeTest, EraseAtBegin) {
    constexpr std::size_t N = 9;

    rbtree<int> tree;
    rbtree<int>::iterator its[N];
    construct_tree<N>(tree, its);

    // check values stored in the tree
    for (std::size_t i = 0; i < N; ++i) {
        ERASE_CHECK(tree, its[i]);
    }

    EXPECT_EQ(0, tree.size());

    EXPECT_EQ(tree.end(), tree.root());
    EXPECT_EQ(tree.end(), tree.begin());
}

TEST(RBTreeTest, EraseAtEnd) {
    constexpr std::size_t N = 9;

    rbtree<int> tree;
    rbtree<int>::iterator its[N];
    construct_tree<N>(tree, its);

    // check values stored in the tree
    for (auto i = N; i > 0; --i) {
        ERASE_CHECK(tree, its[i - 1]);
    }

    EXPECT_EQ(0, tree.size());

    EXPECT_EQ(tree.end(), tree.root());
    EXPECT_EQ(tree.end(), tree.begin());
}

TEST(RBTreeTest, EraseInShuffleOrder) {
    constexpr std::size_t N = 9;

    rbtree<int> tree;
    rbtree<int>::iterator its[N];
    construct_tree<N>(tree, its);

    ERASE_CHECK(tree, its[3]);  // 0 1 2 4 5 6 7 8
    ERASE_CHECK(tree, its[8]);  // 0 1 2 4 5 6 7
    ERASE_CHECK(tree, its[5]);  // 0 1 2 4 6 7
    ERASE_CHECK(tree, its[0]);  // 1 2 4 6 7
    ERASE_CHECK(tree, its[6]);  // 1 2 4 7
    ERASE_CHECK(tree, its[7]);  // 1 2 4
    ERASE_CHECK(tree, its[2]);  // 1 4
    ERASE_CHECK(tree, its[1]);  // 4
    ERASE_CHECK(tree, its[4]);

    EXPECT_EQ(0, tree.size());
    EXPECT_EQ(tree.end(), tree.begin());
    EXPECT_EQ(tree.end(), tree.begin());
}
