/************************************************
 *  tree_list.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_INTERNAL_TREE_LIST_HPP_
#define DICT_INTERNAL_TREE_LIST_HPP_

#include "rbtree.hpp"

namespace dict {

namespace internal {

/************************************************
 * Declaration: class tree_list
 ************************************************/

class tree_list {
 private:  // Private Type(s) - Part 1
    template <bool IsConst> class tree_iterator;

 public:  // Public Type(s)
    using value_type = std::size_t;
    using size_type = std::size_t;
    using reference = value_type &;
    using const_reference = value_type const &;

    using iterator = tree_iterator<false>;
    using const_iterator = tree_iterator<true>;

 public:  // Public Method(s)
    ~tree_list();

    iterator insert(iterator it, value_type val);
    iterator erase(iterator it);

    size_type size() const;
    reference at(size_type i);
    const_reference at(size_type i) const;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    iterator find(size_type i);
    const_iterator find(size_type i) const;

    reference operator[](size_type i);
    const_reference operator[](size_type i) const;

 private:  // Private Type(s) - Part 2
    struct data;
    struct sizes_updater;
    using tree = rbtree<data, sizes_updater>;

 private:  // Private Static Method(s)
    static void update_sizes(typename tree::iterator it);

 private:  // Private Property(ies)
    tree tree_;
};  // class tree_list

/************************************************
 * Declaration: class tree_list::tree_iterator<B>
 ************************************************/

template <bool IsConst>
class tree_list::tree_iterator {
 public:  // Public Type(s)
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename std::conditional<
        IsConst,
        tree_list::value_type const,
        tree_list::value_type
    >::type;
    using pointer = value_type *;
    using reference = value_type &;
    using difference_type = ptrdiff_t;

    using rbtree_iterator = typename std::conditional<
        IsConst,
        tree::const_iterator,
        tree::iterator
    >::type;

 public:  // Public Method(s)
    tree_iterator();
    explicit tree_iterator(rbtree_iterator tree);
    ~tree_iterator();

    rbtree_iterator &get_tree_iterator();
    tree_iterator<false> unconst() const;

    tree_iterator &operator++();
    tree_iterator operator++(int);
    tree_iterator &operator--();
    tree_iterator operator--(int);
    reference operator*();
    pointer operator->();
    bool operator==(tree_iterator it) const;
    bool operator!=(tree_iterator it) const;

    bool operator!() const;
    explicit operator bool() const;

 private:  // Private Property(ies)
    rbtree_iterator it_;
};  // class tree_list::tree_iterator<B>

/************************************************
 * Declaration: struct tree_list::data
 ************************************************/

struct tree_list::data {
    explicit data(value_type x) : val(x), size(1) {
        // do nothing
    }

    value_type val;
    size_type size;
};  // struct tree_list::data

/************************************************
 * Declaration: struct tree_list::sizes_updater
 ************************************************/

struct tree_list::sizes_updater {
    void operator()(typename tree::iterator it) const {
        update_sizes(it);
    }
};  // struct tree_list::sizes_updater

/************************************************
 * Implementation: class tree_list
 ************************************************/

inline tree_list::~tree_list() {
    // do nothing
}

inline tree_list::iterator tree_list::insert(iterator it, value_type val) {
    auto tree_it = tree_.insert_before(
        it.get_tree_iterator(), tree::value_type(val));
    update_sizes(tree_it);
    return decltype(insert(it, val))(tree_it);
}

inline tree_list::iterator tree_list::erase(iterator it) {
    auto erased_it = it.get_tree_iterator();
    auto tree_it = tree_.erase(erased_it);
    return decltype(erase(it))(tree_it);
}

inline tree_list::size_type tree_list::size() const {
    auto root = tree_.root();
    return root ? root->size : 0;
}

inline tree_list::iterator tree_list::begin() {
    return decltype(begin())(tree_.begin());
}

inline tree_list::const_iterator tree_list::begin() const {
    return cbegin();
}

inline tree_list::const_iterator tree_list::cbegin() const {
    return decltype(cbegin())(tree_.cbegin());
}

inline tree_list::iterator tree_list::end() {
    return decltype(end())(tree_.end());
}

inline tree_list::const_iterator tree_list::end() const {
    return cend();
}

inline tree_list::const_iterator tree_list::cend() const {
    return decltype(cend())(tree_.cend());
}

inline tree_list::reference tree_list::at(size_type i) {
    return *(find(i));
}

inline tree_list::const_reference tree_list::at(size_type i) const {
    return *(find(i));
}

inline tree_list::iterator tree_list::find(size_type i) {
    const auto &that = *this;
    return that.find(i).unconst();
}

inline tree_list::const_iterator tree_list::find(size_type i) const {
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

inline tree_list::reference tree_list::operator[](size_type i) {
    return at(i);
}

inline tree_list::const_reference tree_list::operator[](size_type i) const {
    return at(i);
}

inline void tree_list::update_sizes(typename tree::iterator it) {
    do {
        it->size = 1;
        if (it.has_left()) {
            it->size += it.left()->size;
        }

        if (it.has_right()) {
            it->size += it.right()->size;
        }

        it.go_parent();
    } while (it);
}

/************************************************
 * Implementation: class tree_list::tree_iterator<B>
 ************************************************/

template <bool B>
inline tree_list::tree_iterator<B>::tree_iterator() {
    // do nothing
}

template <bool B>
inline tree_list::tree_iterator<B>::tree_iterator(rbtree_iterator it)
    : it_(it) {
    // do nothing
}

template <bool B>
inline tree_list::tree_iterator<B>::~tree_iterator() {
    // do nothing
}

template <bool B>
inline typename tree_list::tree_iterator<B>::rbtree_iterator &
    tree_list::tree_iterator<B>::get_tree_iterator() {
    return it_;
}

template <bool B>
inline typename tree_list::template tree_iterator<false>
tree_list::tree_iterator<B>::unconst() const {
    return tree_iterator<false>(it_.unconst());
}

template <bool B>
inline typename tree_list::template tree_iterator<B> &tree_list::tree_iterator<B>::operator++() {
    ++it_;
    return *this;
}

template <bool B>
inline typename tree_list::template tree_iterator<B> tree_list::tree_iterator<B>::operator++(int) {
    tree_iterator it(it_);
    operator++();
    return it;
}

template <bool B>
inline typename tree_list::template tree_iterator<B> &tree_list::tree_iterator<B>::operator--() {
    --it_;
    return *this;
}

template <bool B>
inline typename tree_list::template tree_iterator<B> tree_list::tree_iterator<B>::operator--(int) {
    tree_iterator it(it_);
    operator--();
    return it;
}

template <bool B>
inline typename tree_list::template tree_iterator<B>::reference
tree_list::tree_iterator<B>::operator*() {
    return *operator->();
}

template <bool B>
inline typename tree_list::template tree_iterator<B>::pointer
tree_list::tree_iterator<B>::operator->() {
    return &(it_->val);
}

template <bool B>
inline bool tree_list::tree_iterator<B>::operator==(tree_iterator it) const {
    return it_ == it.it_;
}

template <bool B>
inline bool tree_list::tree_iterator<B>::operator!=(tree_iterator it) const {
    return !(*this == it);
}

template <bool B>
inline bool tree_list::tree_iterator<B>::operator!() const {
    return !this->operator bool();
}

template <bool B>
inline tree_list::tree_iterator<B>::operator bool() const {
    return static_cast<bool>(it_);
}

}  // namespace internal

}  // namespace dict

#endif  // DICT_INTERNAL_TREE_LIST_HPP_
