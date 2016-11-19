/************************************************
 *  permutation.hpp
 *  DESA
 *
 *  Copyright (c) 2015-2016, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_INTERNAL_PERMUTATION_HPP_
#define DESA_INTERNAL_PERMUTATION_HPP_

#include "rbtree.hpp"

namespace desa {

namespace internal {

/************************************************
 * Declaration: class permutation
 ************************************************/

class permutation {
 public:  // Public Type(s)
    using size_type = std::size_t;
    using value_type = std::size_t;

 public:  // Public Method(s)
    permutation();
    ~permutation();

    void insert(size_type i, size_type j);
    void erase(size_type i);
    void move(size_type from, size_type to);

    size_type size() const;
    size_type at(size_type i) const;
    size_type rank(size_type j) const;

    size_type operator[](size_type i) const;

 private:  // Private Type(s)
    struct link_and_rank;
    using bstree = rbtree<link_and_rank>;

 private:  // Private Static Method(s)
    static typename bstree::const_iterator
        find_node(typename bstree::const_iterator it, size_type i);
    static size_type access(typename bstree::const_iterator it, size_type i);
    static void update_ranks(typename bstree::iterator it);

 private:  // Private Property(ies)
    bstree tree_, inv_tree_;
    size_type size_;
};  // class permutation

/************************************************
 * Declaration: struct permutation::link_and_rank
 ************************************************/

struct permutation::link_and_rank {
    link_and_rank();

    bstree::iterator link;
    size_type rank;
};  // struct permutation::link_and_rank

/************************************************
 * Implementation: class permutation
 ************************************************/

inline permutation::permutation()
    : size_(0) {
    // do nothing
}

inline permutation::~permutation() {
    // do nothing
}

inline permutation::size_type permutation::size() const {
    return size_;
}

inline permutation::size_type permutation::at(size_type i) const {
    return access(tree_.root(), i);
}

inline permutation::size_type permutation::rank(size_type j) const {
    return access(inv_tree_.root(), j);
}

inline permutation::size_type permutation::operator[](size_type i) const {
    return at(i);
}

/************************************************
 * Implementation: struct permutation::link_and_rank
 ************************************************/

inline permutation::link_and_rank::link_and_rank()
    : rank(1) {
    // do nothing
}

}  // namespace internal

}  // namespace desa

#endif  // DESA_INTERNAL_PERMUTATION_HPP_
