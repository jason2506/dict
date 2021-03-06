/************************************************
 *  permutation.cpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include <dict/internal/permutation.hpp>

namespace dict {

namespace internal {

/************************************************
 * Implementation: class permutation
 ************************************************/

void permutation::insert(size_type i, size_type j) {
    auto it = find_node(tree_.croot(), i).unconst();
    it = tree_.insert_before(it, link_and_rank());
    update_ranks(it);

    auto inv_it = find_node(inv_tree_.croot(), j).unconst();
    inv_it = inv_tree_.insert_before(inv_it, link_and_rank());
    update_ranks(inv_it);

    it->link = inv_it;
    inv_it->link = it;
    size_++;
}

void permutation::erase(size_type i) {
    auto it = find_node(tree_.croot(), i).unconst();
    auto inv_it = it->link;

    tree_.erase(it);
    inv_tree_.erase(inv_it);
    size_--;
}

void permutation::move(size_type from, size_type to) {
    auto from_it = find_node(tree_.croot(), from).unconst();
    auto v = *from_it;
    v.rank = 1;

    from_it->rank--;
    update_ranks(from_it.parent());
    auto next_it = tree_.erase(from_it);
    update_ranks(next_it);

    auto to_it = find_node(tree_.croot(), to).unconst();
    auto new_it = tree_.insert_before(to_it, v);
    update_ranks(new_it);

    new_it->link->link = new_it;
}

typename permutation::bstree::const_iterator
permutation::find_node(typename bstree::const_iterator it, size_type i) {
    while (it) {
        if (i < it->rank - 1) {
            it.go_left();
        } else if (i == it->rank - 1) {
            break;
        } else {
            i -= it->rank;
            it.go_right();
        }
    }

    return it;
}

permutation::size_type permutation::access(typename bstree::const_iterator it, size_type i) {
    it = find_node(it, i);
    auto linked_it = it->link;
    auto rank = linked_it->rank - 1;
    auto parent = linked_it.parent();
    while (parent) {
        if (linked_it == parent.right()) {
            rank += parent->rank;
        }

        linked_it.go_parent();
        parent.go_parent();
    }

    return rank;
}

void permutation::update_ranks(typename bstree::iterator it) {
    while (it) {
        it->rank = 1;
        if (it.has_left()) {
            auto left_it = it.left();
            it->rank += left_it->rank;
            while (left_it.has_right()) {
                left_it.go_right();
                it->rank += left_it->rank;
            }
        }

        it.go_parent();
    }
}

}  // namespace internal

}  // namespace dict
