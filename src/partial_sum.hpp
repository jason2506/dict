/************************************************
 *  partial_sum.hpp
 *  ESA++
 *
 *  Copyright (c) 2014-2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef ESAPP_PARTIAL_SUM_HPP_
#define ESAPP_PARTIAL_SUM_HPP_

#include <functional>
#include <stdexcept>

#include "rbtree.hpp"

namespace esapp
{

namespace impl
{

/************************************************
 * Declaration: class partial_sum<K, T>
 ************************************************/

template <typename Key, typename T>
class partial_sum
{
public: // Public Type(s)
    typedef Key key_type;
    typedef T value_type;

public: // Public Method(s)
    ~partial_sum(void);

    void increase(key_type k, value_type x);
    void decrease(key_type k, value_type x);

    value_type sum(void) const;
    value_type sum(key_type k) const;
    key_type search(value_type x) const;

private: // Private Type(s)
    struct key_and_sum;
    typedef rbtree<key_and_sum> bstree;

private: // Private Static Method(s)
    template <typename Op>
    void update(key_type k, value_type x, Op op);
    static void update_sums(typename bstree::iterator it);

private: // Private Property(ies)
    bstree tree_;
}; // class partial_sum<K, T>

/************************************************
 * Declaration: struct partial_sum<K, T>::key_and_sum
 ************************************************/

template <typename K, typename T>
struct partial_sum<K, T>::key_and_sum
{
    key_type key;
    value_type sum;
}; // struct partial_sum<K, T>::sum

/************************************************
 * Implementation: class partial_sum<K, T>
 ************************************************/

template <typename K, typename T>
inline partial_sum<K, T>::~partial_sum(void)
{
    // do nothing
}

template <typename K, typename T>
inline void partial_sum<K, T>::increase(key_type k, value_type x)
{
    update(k, x, ::std::plus<T>());
}

template <typename K, typename T>
inline void partial_sum<K, T>::decrease(key_type k, value_type x)
{
    update(k, x, ::std::minus<T>());
}

template <typename K, typename T>
typename partial_sum<K, T>::value_type partial_sum<K, T>::sum(void) const
{
    auto it = tree_.root();
    auto sum = 0;
    while (it)
    {
        sum += it->sum;
        it.go_right();
    }

    return sum;
}

template <typename K, typename T>
typename partial_sum<K, T>::value_type partial_sum<K, T>::sum(key_type k) const
{
    value_type sum = 0;
    auto it = tree_.root();
    while (it)
    {
        if (k < it->key) { it.go_left(); }
        else
        {
            sum += it->sum;
            if (k == it->key)   { break; }
            else                { it.go_right(); }
        }
    }

    return sum;
}

template <typename K, typename T>
typename partial_sum<K, T>::key_type partial_sum<K, T>::search(value_type x) const
{
    auto it = tree_.root();

    bool found = false;
    key_type result;
    while (it)
    {
        if (x > it->sum)
        {
            x -= it->sum;
            it.go_right();
        }
        else
        {
            result = it->key;
            found = true;

            it.go_left();
        }
    }

    if (!found)
    {
        throw ::std::invalid_argument("not found");
    }

    return result;
}

template <typename K, typename T>
template <typename Op>
void partial_sum<K, T>::update(key_type k, value_type x, Op op)
{
    auto it = tree_.root();
    if (!it)
    {
        tree_.insert_before(it, {k, op(0, x)}, update_sums);
        return;
    }

    while (1)
    {
        if (k == it->key)
        {
            it->sum = op(it->sum, x);
            break;
        }
        else if (k < it->key)
        {
            it->sum = op(it->sum, x);
            if (it.has_left()) { it.go_left(); }
            else
            {
                tree_.insert_before(it, {k, op(0, x)}, update_sums);
                break;
            }
        }
        else
        {
            if (it.has_right()) { it.go_right(); }
            else
            {
                tree_.insert_before(++it, {k, op(0, x)}, update_sums);
                break;
            }
        }
    }
}

template <typename K, typename T>
inline void partial_sum<K, T>::update_sums(typename bstree::iterator it)
{
    if (it.has_parent())
    {
        if (it == it.parent().left())
        {
            it.parent()->sum += it->sum;
        }
        else
        {
            it->sum -= it.parent()->sum;
        }
    }
}

} // namespace impl

} // namespace esapp

#endif // ESAPP_PARTIAL_SUM_HPP_
