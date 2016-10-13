/************************************************
 *  tree_list.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_TREE_LIST_HPP_
#define DESA_TREE_LIST_HPP_

#include "rbtree.hpp"

namespace desa
{

namespace impl
{

/************************************************
 * Declaration: class tree_list
 ************************************************/

class tree_list
{
private: // Private Type(s) - Part 1
    template <bool IsConst> class tree_iterator;

public: // Public Type(s)
    typedef ::std::size_t value_type;
    typedef ::std::size_t size_type;
    typedef value_type &reference;
    typedef value_type const &const_reference;

    typedef tree_iterator<false> iterator;
    typedef tree_iterator<true> const_iterator;

public: // Public Method(s)
    ~tree_list(void);

    iterator insert(iterator it, value_type val);
    iterator erase(iterator it);

    size_type size(void) const;
    reference at(size_type i);
    const_reference at(size_type i) const;

    iterator begin(void);
    const_iterator begin(void) const;
    const_iterator cbegin(void) const;
    iterator end(void);
    const_iterator end(void) const;
    const_iterator cend(void) const;

    iterator find(size_type i);
    const_iterator find(size_type i) const;

    reference operator[](size_type i);
    const_reference operator[](size_type i) const;

private: // Private Type(s) - Part 2
    struct data;
    typedef rbtree<data> tree;

private: // Private Static Method(s)
    static void update_sizes(typename tree::iterator it);

private: // Private Property(ies)
    tree tree_;
}; // class tree_list

/************************************************
 * Declaration: class tree_list::tree_iterator<B>
 ************************************************/

template <bool IsConst>
class tree_list::tree_iterator
{
public: // Public Type(s)
    typedef ::std::bidirectional_iterator_tag iterator_category;
    typedef typename ::std::conditional<IsConst, value_type const, value_type>::type value_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef ptrdiff_t difference_type;

    typedef typename ::std::conditional
        <
            IsConst,
            tree::const_iterator,
            tree::iterator
        >::type rbtree_iterator;

public: // Public Method(s)
    tree_iterator(void);
    explicit tree_iterator(rbtree_iterator tree);
    ~tree_iterator(void);

    rbtree_iterator &get_tree_iterator(void);
    tree_iterator<false> unconst(void) const;

    tree_iterator &operator++(void);
    tree_iterator operator++(int);
    tree_iterator &operator--(void);
    tree_iterator operator--(int);
    reference operator*(void);
    pointer operator->(void);
    bool operator==(tree_iterator it) const;
    bool operator!=(tree_iterator it) const;

    bool operator!(void) const;
    explicit operator bool(void) const;

private: // Private Property(ies)
    rbtree_iterator it_;
}; // class tree_list::tree_iterator<B>

/************************************************
 * Declaration: struct tree_list::data
 ************************************************/

struct tree_list::data
{
    explicit data(value_type x);

    value_type val;
    size_type size;
}; // struct tree_list::data

/************************************************
 * Implementation: class tree_list
 ************************************************/

inline tree_list::~tree_list(void)
{
    // do nothing
}

inline tree_list::iterator tree_list::insert(iterator it, value_type val)
{
    auto tree_it = tree_.insert_before(it.get_tree_iterator(), tree::value_type(val), update_sizes);
    update_sizes(tree_it);
    return decltype(insert(it, val))(tree_it);
}

inline tree_list::iterator tree_list::erase(iterator it)
{
    auto tree_it = tree_.erase(it.get_tree_iterator(), update_sizes);
    update_sizes(tree_it);
    return decltype(erase(it))(tree_it);
}

inline tree_list::size_type tree_list::size(void) const
{
    auto root = tree_.root();
    return root ? root->size : 0;
}

inline tree_list::iterator tree_list::begin(void)
{
    return decltype(begin())(tree_.begin());
}

inline tree_list::const_iterator tree_list::begin(void) const
{
    return cbegin();
}

inline tree_list::const_iterator tree_list::cbegin(void) const
{
    return decltype(cbegin())(tree_.cbegin());
}

inline tree_list::iterator tree_list::end(void)
{
    return decltype(end())(tree_.end());
}

inline tree_list::const_iterator tree_list::end(void) const
{
    return cend();
}

inline tree_list::const_iterator tree_list::cend(void) const
{
    return decltype(cend())(tree_.cend());
}

inline tree_list::reference tree_list::at(size_type i)
{
    return *(find(i));
}

inline tree_list::const_reference tree_list::at(size_type i) const
{
    return *(find(i));
}

inline tree_list::iterator tree_list::find(size_type i)
{
    const auto &that = *this;
    return that.find(i).unconst();
}

inline tree_list::const_iterator tree_list::find(size_type i) const
{
    auto it = tree_.root();
    while (it) {
        auto left_size = it.has_left() ? it.left()->size : 0;
        if (i == left_size) {
            break;
        } else if (i < left_size) {
            it.go_left();
        } else {
            it.go_right();
            i -= left_size + 1;
        }
    }

    return decltype(find(i))(it);
}

inline tree_list::reference tree_list::operator[](size_type i)
{
    return at(i);
}

inline tree_list::const_reference tree_list::operator[](size_type i) const
{
    return at(i);
}

inline void tree_list::update_sizes(typename tree::iterator it)
{
    while (it)
    {
        it->size = 1;
        if (it.has_left())
        {
            it->size += it.left()->size;
        }

        if (it.has_right())
        {
            it->size += it.right()->size;
        }

        it.go_parent();
    }
}

/************************************************
 * Implementation: class tree_list::tree_iterator<B>
 ************************************************/

template <bool B>
inline tree_list::tree_iterator<B>::tree_iterator(void)
{
    // do nothing
}

template <bool B>
inline tree_list::tree_iterator<B>::tree_iterator(rbtree_iterator it)
    : it_(it)
{
    // do nothing
}

template <bool B>
inline tree_list::tree_iterator<B>::~tree_iterator(void)
{
    // do nothing
}

template <bool B>
inline typename tree_list::tree_iterator<B>::rbtree_iterator &
    tree_list::tree_iterator<B>::get_tree_iterator(void)
{
    return it_;
}

template <bool B>
inline typename tree_list::template tree_iterator<false> tree_list::tree_iterator<B>::unconst(void) const
{
    return tree_iterator<false>(it_.unconst());
}

template <bool B>
inline typename tree_list::template tree_iterator<B> &tree_list::tree_iterator<B>::operator++(void)
{
    ++it_;
    return *this;
}

template <bool B>
inline typename tree_list::template tree_iterator<B> tree_list::tree_iterator<B>::operator++(int)
{
    tree_iterator it(it_);
    operator++();
    return it;
}

template <bool B>
inline typename tree_list::template tree_iterator<B> &tree_list::tree_iterator<B>::operator--(void)
{
    --it_;
    return *this;
}

template <bool B>
inline typename tree_list::template tree_iterator<B> tree_list::tree_iterator<B>::operator--(int)
{
    tree_iterator it(it_);
    operator--();
    return it;
}

template <bool B>
inline typename tree_list::template tree_iterator<B>::reference tree_list::tree_iterator<B>::operator*(void)
{
    return *operator->();
}

template <bool B>
inline typename tree_list::template tree_iterator<B>::pointer tree_list::tree_iterator<B>::operator->(void)
{
    return &(it_->val);
}

template <bool B>
inline bool tree_list::tree_iterator<B>::operator==(tree_iterator it) const
{
    return it_ == it.it_;
}

template <bool B>
inline bool tree_list::tree_iterator<B>::operator!=(tree_iterator it) const
{
    return !(*this == it);
}

template <bool B>
inline bool tree_list::tree_iterator<B>::operator!(void) const
{
    return !this->operator bool();
}

template <bool B>
inline tree_list::tree_iterator<B>::operator bool(void) const
{
    return static_cast<bool>(it_);
}

/************************************************
 * Implementation: struct tree_list::data
 ************************************************/

inline tree_list::data::data(value_type x)
    : val(x), size(1)
{
    // do nothing
}

} // namespace impl

} // namespace desa

#endif // DESA_TREE_LIST_HPP_
