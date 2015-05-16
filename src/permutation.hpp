/************************************************
 *  permutation.hpp
 *  ESA++
 *
 *  Copyright (c) 2014-2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef ESAPP_PERMUTATION_HPP_
#define ESAPP_PERMUTATION_HPP_

#include "rbtree.hpp"

namespace esapp
{

namespace impl
{

/************************************************
 * Declaration: class permutation
 ************************************************/

class permutation
{
public: // Public Type(s)
    typedef ::std::size_t value_type;
    typedef ::std::size_t size_type;

public: // Public Method(s)
    permutation(void);
    ~permutation(void);

    void insert(size_type i, size_type j);
    void erase(size_type i);
    void move(size_type from, size_type to);

    size_type size(void) const;
    size_type at(size_type i) const;
    size_type rank(size_type j) const;

    size_type operator[](size_t i) const;

private: // Private Type(s)
    struct link_and_rank;
    typedef rbtree<link_and_rank> bstree;

private: // Private Static Method(s)
    static typename bstree::const_iterator find_node(typename bstree::const_iterator it, size_type i);
    static size_type access(typename bstree::const_iterator it, size_type i);
    static void update_ranks(typename bstree::iterator it);

private: // Private Property(ies)
    bstree tree_, inv_tree_;
    size_type size_;
}; // class permutation

/************************************************
 * Declaration: struct permutation::link_and_rank
 ************************************************/

struct permutation::link_and_rank
{
    link_and_rank(void);

    bstree::iterator link;
    size_type rank;
}; // struct permutation::link_and_rank

/************************************************
 * Implementation: class permutation
 ************************************************/

inline permutation::permutation(void)
    : size_(0)
{
    // do nothing
}

inline permutation::~permutation(void)
{
    // do nothing
}

inline permutation::size_type permutation::size(void) const
{
    return size_;
}

inline permutation::size_type permutation::at(size_type i) const
{
    return access(tree_.root(), i);
}

inline permutation::size_type permutation::rank(size_type j) const
{
    return access(inv_tree_.root(), j);
}

inline permutation::size_type permutation::operator[](size_type i) const
{
    return at(i);
}

/************************************************
 * Implementation: struct permutation::link_and_rank
 ************************************************/

inline permutation::link_and_rank::link_and_rank(void)
    : rank(1)
{
    // do nothing
}

} // namespace impl

} // namespace esapp

#endif // ESAPP_PERMUTATION_HPP_
