/************************************************
 *  rbtree.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_RBTREE_HPP_
#define DESA_RBTREE_HPP_

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace desa
{

namespace impl
{

/************************************************
 * Declaration: class rbtree<T>
 ************************************************/

template <typename T>
class rbtree
{
private: // Private Type(s) - Part 1
    template <bool IsConst> class tree_iterator;

public: // Public Type(s)
    typedef T value_type;

    typedef tree_iterator<false> iterator;
    typedef tree_iterator<true> const_iterator;

public: // Public Method(s)
    rbtree(void);
    ~rbtree(void);

    iterator root(void);
    const_iterator root(void) const;
    iterator begin(void);
    const_iterator begin(void) const;
    const_iterator cbegin(void) const;
    iterator end(void);
    const_iterator end(void) const;
    const_iterator cend(void) const;

    template <typename Update>
    iterator insert_before(iterator pos, value_type const &data, Update const &update);
    template <typename Update>
    iterator erase(iterator pos, Update const &update);

private: // Private Type(s) - Part 2
    enum color {RED, BLACK};
    class node;
    typedef node *weak_node_ptr;
    typedef node const *weak_const_node_ptr;
    typedef ::std::unique_ptr<node> node_ptr;
    typedef ::std::unique_ptr<node const> const_node_ptr;

private: // Private Method(s)
    template <typename Update>
    void rebalance_after_insertion(weak_node_ptr ptr, weak_node_ptr parent, Update const &update);
    template <typename Update>
    void rebalance_after_erasure(weak_node_ptr ptr, weak_node_ptr parent, Update const &update);

    template <typename Update>
    void rotate_left(weak_node_ptr weak_ptr, Update const &update);
    template <typename Update>
    void rotate_right(weak_node_ptr weak_ptr, Update const &update);

    weak_node_ptr next_node(weak_node_ptr ptr) const;
    weak_node_ptr prev_node(weak_node_ptr ptr) const;

    static bool is_red_node(weak_const_node_ptr ptr);
    static bool is_black_node(weak_const_node_ptr ptr);

private: // Private Property(ies)
    node_ptr root_;
    weak_node_ptr first_, last_;
}; // class rbtree<T>

/************************************************
 * Declaration: class rbtree<T>::node
 ************************************************/

template <typename T>
class rbtree<T>::node
{
public: // Public Method(s)
    explicit node(value_type const &data);
    ~node(void);

    weak_const_node_ptr get_parent(void) const;
    weak_node_ptr get_parent(void);
    void set_parent(weak_node_ptr parent);

    weak_const_node_ptr get_left(void) const;
    weak_node_ptr get_left(void);
    void set_left(weak_node_ptr left);
    void set_left(node_ptr &&left);
    node_ptr move_left(void);

    weak_const_node_ptr get_right(void) const;
    weak_node_ptr get_right(void);
    void set_right(weak_node_ptr right);
    void set_right(node_ptr &&right);
    node_ptr move_right(void);

    color get_color(void) const;
    void set_color(color c);

    value_type const &data(void) const;
    value_type &data(void);

private: // Private Property(ies)
    weak_node_ptr parent_;
    node_ptr left_, right_;
    color color_;
    value_type data_;
}; // class rbtree<T>::node

/************************************************
 * Declaration: class rbtree<T>::tree_iterator<B>
 ************************************************/

template <typename T>
template <bool IsConst>
class rbtree<T>::tree_iterator
{
public: // Public Type(s)
    typedef ::std::bidirectional_iterator_tag iterator_category;
    typedef typename ::std::conditional
        <
            IsConst,
            rbtree<T>::value_type const,
            rbtree<T>::value_type
        >::type value_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef ptrdiff_t difference_type;

    typedef typename ::std::conditional
        <
            IsConst,
            weak_const_node_ptr,
            weak_node_ptr
        >::type node_ptr;

public: // Public Method(s)
    tree_iterator(void);
    explicit tree_iterator(rbtree const *tree);
    tree_iterator(rbtree const *tree, node_ptr ptr);
    tree_iterator(rbtree const *tree, typename rbtree<T>::node_ptr const &ptr);
    ~tree_iterator(void);

    bool has_parent(void);
    bool has_left(void);
    bool has_right(void);

    void go_parent(void);
    void go_left(void);
    void go_right(void);

    tree_iterator parent(void);
    tree_iterator left(void);
    tree_iterator right(void);

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

    node_ptr get_node_ptr(void);

private: // Private Property(ies)
    rbtree const *tree_;
    node_ptr ptr_;
}; // class rbtree<T>::tree_iterator<B>

/************************************************
 * Implementation: class rbtree<T>
 ************************************************/

template <typename T>
inline rbtree<T>::rbtree(void) : first_(nullptr), last_(nullptr)
{
    // do nothing
}

template <typename T>
inline rbtree<T>::~rbtree(void)
{
    // do nothing
}

template <typename T>
inline typename rbtree<T>::iterator rbtree<T>::root(void)
{
    return iterator(this, root_);
}

template <typename T>
inline typename rbtree<T>::const_iterator rbtree<T>::root(void) const
{
    return const_iterator(this, root_);
}

template <typename T>
inline typename rbtree<T>::iterator rbtree<T>::begin(void)
{
    return iterator(this, first_);
}

template <typename T>
inline typename rbtree<T>::const_iterator rbtree<T>::begin(void) const
{
    return cbegin();
}

template <typename T>
inline typename rbtree<T>::const_iterator rbtree<T>::cbegin(void) const
{
    return const_iterator(this, first_);
}

template <typename T>
inline typename rbtree<T>::iterator rbtree<T>::end(void)
{
    return iterator(this);
}

template <typename T>
inline typename rbtree<T>::const_iterator rbtree<T>::end(void) const
{
    return cend();
}

template <typename T>
inline typename rbtree<T>::const_iterator rbtree<T>::cend(void) const
{
    return const_iterator(this);
}

template <typename T>
template <typename Update>
typename rbtree<T>::iterator rbtree<T>::insert_before(iterator pos, value_type const &data, Update const &update)
{
    auto new_node = new node(data);
    auto ptr = pos.get_node_ptr();
    if (ptr)
    {
        if (ptr->get_left())
        {
            ptr = ptr->get_left();
            while (ptr->get_right())
            {
                ptr = ptr->get_right();
            }

            ptr->set_right(new_node);
        }
        else
        {
            if (ptr == first_) { first_ = new_node; }

            ptr->set_left(new_node);
        }

        new_node->set_parent(ptr);
        new_node->set_color(RED);
    }
    else if (last_)
    {
        // insert after the last node
        new_node->set_parent(last_);
        new_node->set_color(RED);
        last_->set_right(new_node);
        last_ = new_node;
    }
    else
    {
        // current node is at the root of the tree
        first_ = last_ = new_node;
        root_.reset(new_node);
        root_->set_color(BLACK);
        return iterator(this, root_);
    }

    rebalance_after_insertion(new_node, new_node->get_parent(), update);
    return iterator(this, new_node);
}

template <typename T>
template <typename Update>
typename rbtree<T>::iterator rbtree<T>::erase(iterator pos, Update const &update)
{
    auto ptr = pos.get_node_ptr();
    if (first_ == ptr && last_ == ptr)
    {
        // current node is at the root of the tree
        root_.release();
        first_ = last_ = nullptr;
        return end();
    }

    auto parent_it = pos.parent();
    ++pos;
    auto next_parent_it = pos ? pos.parent() : end();

    auto next_ptr = pos.get_node_ptr();
    if (ptr == first_)  { first_ = next_ptr; }
    if (ptr == last_)   { last_ = prev_node(ptr); }

    auto ptr_color = ptr->get_color();
    auto parent = ptr->get_parent();
    weak_node_ptr child;
    if (ptr->get_left() && ptr->get_right())
    {
        // current node has two non-null children
        ptr_color = next_ptr->get_color();
        next_ptr->set_color(ptr->get_color());

        next_ptr->set_left(ptr->move_left());
        next_ptr->get_left()->set_parent(next_ptr);

        auto next_ptr_parent = next_ptr->get_parent();
        next_ptr->set_parent(parent);
        child = next_ptr->get_right();
        if (ptr == next_ptr_parent)
        {
            if (!parent)
            {
                root_ = ptr->move_right();
            }
            else if (ptr == parent->get_left())
            {
                parent->set_left(ptr->move_right());
            }
            else
            {
                parent->set_right(ptr->move_right());
            }

            parent = next_ptr;
        }
        else
        {
            auto child_uptr = next_ptr->move_right();
            next_ptr->set_right(ptr->move_right());
            next_ptr->get_right()->set_parent(next_ptr);
            if (!parent)
            {
                root_ = next_ptr_parent->move_left();
            }
            else if (ptr == parent->get_left())
            {
                parent->set_left(next_ptr_parent->move_left());
            }
            else
            {
                parent->set_right(next_ptr_parent->move_left());
            }

            next_ptr_parent->set_left(::std::move(child_uptr));
            parent = next_ptr_parent;
        }
    }
    else
    {
        // current node has at most one non-null child
        node_ptr child_uptr;
        if (!ptr->get_left())   { child_uptr = ptr->move_right(); }
        else                    { child_uptr = ptr->move_left(); }

        // replace current node with its child
        child = child_uptr.get();
        if (!parent)
        {
            root_ = ::std::move(child_uptr);
        }
        else if (ptr == parent->get_left())
        {
            parent->set_left(::std::move(child_uptr));
        }
        else
        {
            parent->set_right(::std::move(child_uptr));
        }
    }

    if (child)
    {
        child->set_parent(parent);
    }

    if (ptr == next_parent_it.get_node_ptr())
    {
        update(pos);
    }
    else if (pos && pos.parent() != next_parent_it)
    {
        update(next_parent_it);
    }
    else
    {
        update(parent_it);
    }

    if (ptr_color == BLACK)
    {
        rebalance_after_erasure(child, parent, update);
    }

    return iterator(this, next_ptr);
}

template <typename T>
template <typename Update>
void rbtree<T>::rebalance_after_insertion(weak_node_ptr ptr, weak_node_ptr parent, Update const &update)
{
    while (is_red_node(parent))
    {
        auto grandparent = parent->get_parent();
        auto uncle = grandparent
            ? (parent == grandparent->get_left())
                ? grandparent->get_right()
                : grandparent->get_left()
            : nullptr;
        if (is_red_node(uncle))
        {
            // Case 1
            parent->set_color(BLACK);
            uncle->set_color(BLACK);
            grandparent->set_color(RED);
            ptr = grandparent;
            parent = ptr->get_parent();
        }
        else
        {
            // Case 2
            if (ptr == parent->get_right() &&
                parent == grandparent->get_left())
            {
                rotate_left(parent, update);
                ptr = ptr->get_left();
                parent = ptr->get_parent();
            }
            else if (ptr == parent->get_left() &&
                     parent == grandparent->get_right())
            {
                rotate_right(parent, update);
                ptr = ptr->get_right();
                parent = ptr->get_parent();
            }

            // Case 3
            parent->set_color(BLACK);
            grandparent->set_color(RED);
            if (ptr == parent->get_left())
            {
                rotate_right(grandparent, update);
            }
            else
            {
                rotate_left(grandparent, update);
            }
        }
    }

    root_->set_color(BLACK);
}

template <typename T>
template <typename Update>
void rbtree<T>::rebalance_after_erasure(weak_node_ptr ptr, weak_node_ptr parent, Update const &update)
{
    while (parent && is_black_node(ptr))
    {
        auto sibling = (ptr == parent->get_left())
            ? parent->get_right()
            : parent->get_left();
        if (sibling->get_color() == RED)
        {
            // Case 1
            sibling->set_color(BLACK);
            parent->set_color(RED);
            if (ptr == parent->get_left())
            {
                rotate_left(parent, update);
                sibling = parent->get_right();
            }
            else
            {
                rotate_right(parent, update);
                sibling = parent->get_left();
            }
        }

        if (is_black_node(sibling->get_left()) &&
            is_black_node(sibling->get_right()))
        {
            // Case 2
            sibling->set_color(RED);
            ptr = parent;
            parent = parent->get_parent();
        }
        else if (ptr == parent->get_left())
        {
            if (is_black_node(sibling->get_right()))
            {
                // Case 3
                if (sibling->get_left())
                {
                    sibling->get_left()->set_color(BLACK);
                }

                sibling->set_color(RED);
                rotate_right(sibling, update);
                sibling = parent->get_right();
            }

            // Case 4
            sibling->set_color(parent->get_color());
            parent->set_color(BLACK);
            sibling->get_right()->set_color(BLACK);
            rotate_left(parent, update);
            ptr = root_.get();
            break;
        }
        else
        {
            if (is_black_node(sibling->get_left()))
            {
                // Case 3
                if (sibling->get_right())
                {
                    sibling->get_right()->set_color(BLACK);
                }

                sibling->set_color(RED);
                rotate_left(sibling, update);
                sibling = parent->get_left();
            }

            // Case 4
            sibling->set_color(parent->get_color());
            parent->set_color(BLACK);
            sibling->get_left()->set_color(BLACK);
            rotate_right(parent, update);
            ptr = root_.get();
            break;
        }
    }

    if (ptr)
    {
        ptr->set_color(BLACK);
    }
}

template <typename T>
template <typename Update>
void rbtree<T>::rotate_left(weak_node_ptr ptr, Update const &update)
{
    auto parent = ptr->get_parent();
    auto right = ptr->move_right();
    right->set_parent(parent);
    ptr->set_parent(right.get());

    ptr->set_right(right->move_left());
    if (!parent)
    {
        right->set_left(::std::move(root_));
        root_ = ::std::move(right);
    }
    else if (ptr == parent->get_left())
    {
        right->set_left(parent->move_left());
        parent->set_left(::std::move(right));
    }
    else
    {
        right->set_left(parent->move_right());
        parent->set_right(::std::move(right));
    }

    if (ptr->get_right())
    {
        ptr->get_right()->set_parent(ptr);
    }

    update(iterator(this, ptr));
}

template <typename T>
template <typename Update>
void rbtree<T>::rotate_right(weak_node_ptr ptr, Update const &update)
{
    auto parent = ptr->get_parent();
    auto left = ptr->move_left();
    left->set_parent(parent);
    ptr->set_parent(left.get());

    ptr->set_left(left->move_right());
    if (!parent)
    {
        left->set_right(::std::move(root_));
        root_ = ::std::move(left);
    }
    else if (ptr == parent->get_left())
    {
        left->set_right(parent->move_left());
        parent->set_left(::std::move(left));
    }
    else
    {
        left->set_right(parent->move_right());
        parent->set_right(::std::move(left));
    }

    if (ptr->get_left())
    {
        ptr->get_left()->set_parent(ptr);
    }

    update(iterator(this, ptr));
}

template <typename T>
typename rbtree<T>::weak_node_ptr rbtree<T>::next_node(weak_node_ptr ptr) const
{
    if (!ptr) { ptr = first_; }
    else if (ptr == last_) { ptr = nullptr; }
    else if (ptr->get_right())
    {
        ptr = ptr->get_right();
        while (ptr->get_left())
        {
            ptr = ptr->get_left();
        }
    }
    else
    {
        while (ptr == ptr->get_parent()->get_right())
        {
            ptr = ptr->get_parent();
        }

        ptr = ptr->get_parent();
    }

    return ptr;
}

template <typename T>
typename rbtree<T>::weak_node_ptr rbtree<T>::prev_node(weak_node_ptr ptr) const
{
    if (!ptr) { ptr = last_; }
    else if (ptr == first_) { ptr = nullptr; }
    else if (ptr->get_left())
    {
        ptr = ptr->get_left();
        while (ptr->get_right())
        {
            ptr = ptr->get_right();
        }
    }
    else
    {
        while (ptr == ptr->get_parent()->get_left())
        {
            ptr = ptr->get_parent();
        }

        ptr = ptr->get_parent();
    }

    return ptr;
}

template <typename T>
inline bool rbtree<T>::is_red_node(weak_const_node_ptr ptr)
{
    return ptr && ptr->get_color() == RED;
}

template <typename T>
inline bool rbtree<T>::is_black_node(weak_const_node_ptr ptr)
{
    return !is_red_node(ptr);
}

/************************************************
 * Implementation: struct rbtree<T>::node
 ************************************************/

template <typename T>
inline rbtree<T>::node::node(T const &data)
    : parent_(nullptr), data_(data)
{
    // do nothing
}

template <typename T>
inline rbtree<T>::node::~node(void)
{
    // do nothing
}

template <typename T>
inline typename rbtree<T>::weak_const_node_ptr rbtree<T>::node::get_parent(void) const
{
    return parent_;
}

template <typename T>
inline typename rbtree<T>::weak_node_ptr rbtree<T>::node::get_parent(void)
{
    return parent_;
}

template <typename T>
inline void rbtree<T>::node::set_parent(weak_node_ptr parent)
{
    parent_ = parent;
}

template <typename T>
inline typename rbtree<T>::weak_const_node_ptr rbtree<T>::node::get_left(void) const
{
    return left_.get();
}

template <typename T>
inline typename rbtree<T>::weak_node_ptr rbtree<T>::node::get_left(void)
{
    return left_.get();
}

template <typename T>
inline void rbtree<T>::node::set_left(weak_node_ptr left)
{
    left_.reset(left);
}

template <typename T>
inline void rbtree<T>::node::set_left(node_ptr &&left)
{
    left_ = ::std::move(left);
}

template <typename T>
inline typename rbtree<T>::node_ptr rbtree<T>::node::move_left(void)
{
    return ::std::move(left_);
}

template <typename T>
inline typename rbtree<T>::weak_const_node_ptr rbtree<T>::node::get_right(void) const
{
    return right_.get();
}

template <typename T>
inline typename rbtree<T>::weak_node_ptr rbtree<T>::node::get_right(void)
{
    return right_.get();
}

template <typename T>
inline void rbtree<T>::node::set_right(weak_node_ptr right)
{
    right_.reset(right);
}

template <typename T>
inline void rbtree<T>::node::set_right(node_ptr &&right)
{
    right_ = ::std::move(right);
}

template <typename T>
inline typename rbtree<T>::node_ptr rbtree<T>::node::move_right(void)
{
    return ::std::move(right_);
}

template <typename T>
inline typename rbtree<T>::color rbtree<T>::node::get_color(void) const
{
    return color_;
}

template <typename T>
inline void rbtree<T>::node::set_color(color c)
{
    color_ = c;
}

template <typename T>
inline T const &rbtree<T>::node::data(void) const
{
    return data_;
}

template <typename T>
inline T &rbtree<T>::node::data(void)
{
    return data_;
}

/************************************************
 * Implementation: class rbtree<T>::tree_iterator<B>
 ************************************************/

template <typename T>
template <bool B>
inline rbtree<T>::tree_iterator<B>::tree_iterator(void)
    : tree_(nullptr), ptr_(nullptr)
{
    // do nothing
}

template <typename T>
template <bool B>
inline rbtree<T>::tree_iterator<B>::tree_iterator(rbtree const *tree)
    : tree_(tree), ptr_(nullptr)
{
    // do nothing
}

template <typename T>
template <bool B>
inline rbtree<T>::tree_iterator<B>::tree_iterator(rbtree const *tree, node_ptr ptr)
    : tree_(tree), ptr_(ptr)
{
    // do nothing
}

template <typename T>
template <bool B>
inline rbtree<T>::tree_iterator<B>::tree_iterator(rbtree const *tree, typename rbtree<T>::node_ptr const &ptr)
    : tree_(tree), ptr_(ptr.get())
{
    // do nothing
}

template <typename T>
template <bool B>
inline rbtree<T>::tree_iterator<B>::~tree_iterator(void)
{
    // do nothing
}

template <typename T>
template <bool B>
inline bool rbtree<T>::tree_iterator<B>::has_parent(void)
{
    return ptr_->get_parent();
}

template <typename T>
template <bool B>
inline bool rbtree<T>::tree_iterator<B>::has_left(void)
{
    return ptr_->get_left();
}

template <typename T>
template <bool B>
inline bool rbtree<T>::tree_iterator<B>::has_right(void)
{
    return ptr_->get_right();
}

template <typename T>
template <bool B>
inline void rbtree<T>::tree_iterator<B>::go_parent(void)
{
    ptr_ = ptr_->get_parent();
}

template <typename T>
template <bool B>
inline void rbtree<T>::tree_iterator<B>::go_left(void)
{
    ptr_ = ptr_->get_left();
}

template <typename T>
template <bool B>
inline void rbtree<T>::tree_iterator<B>::go_right(void)
{
    ptr_ = ptr_->get_right();
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<B> rbtree<T>::tree_iterator<B>::parent(void)
{
    return tree_iterator(tree_, ptr_->get_parent());
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<B> rbtree<T>::tree_iterator<B>::left(void)
{
    return tree_iterator(tree_, ptr_->get_left());
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<B> rbtree<T>::tree_iterator<B>::right(void)
{
    return tree_iterator(tree_, ptr_->get_right());
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<false> rbtree<T>::tree_iterator<B>::unconst(void) const
{
    typedef typename tree_iterator<false>::node_ptr node_ptr;
    return tree_iterator<false>(tree_, const_cast<node_ptr>(ptr_));
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<B> &rbtree<T>::tree_iterator<B>::operator++(void)
{
    ptr_ = tree_->next_node(ptr_);
    return *this;
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<B> rbtree<T>::tree_iterator<B>::operator++(int)
{
    tree_iterator it(tree_, ptr_);
    operator++();
    return it;
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<B> &rbtree<T>::tree_iterator<B>::operator--(void)
{
    ptr_ = tree_->prev_node(ptr_);
    return *this;
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<B> rbtree<T>::tree_iterator<B>::operator--(int)
{
    tree_iterator it(tree_, ptr_);
    operator--();
    return it;
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<B>::reference rbtree<T>::tree_iterator<B>::operator*(void)
{
    return *operator->();
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<B>::pointer rbtree<T>::tree_iterator<B>::operator->(void)
{
    return &ptr_->data();
}

template <typename T>
template <bool B>
inline bool rbtree<T>::tree_iterator<B>::operator==(tree_iterator it) const
{
    return ptr_ == it.ptr_;
}

template <typename T>
template <bool B>
inline bool rbtree<T>::tree_iterator<B>::operator!=(tree_iterator it) const
{
    return !(*this == it);
}

template <typename T>
template <bool B>
inline bool rbtree<T>::tree_iterator<B>::operator!(void) const
{
    return !this->operator bool();
}

template <typename T>
template <bool B>
inline rbtree<T>::tree_iterator<B>::operator bool(void) const
{
    return static_cast<bool>(ptr_);
}

template <typename T>
template <bool B>
inline typename rbtree<T>::template tree_iterator<B>::node_ptr rbtree<T>::tree_iterator<B>::get_node_ptr(void)
{
    return ptr_;
}

} // namespace impl

} // namespace desa

#endif // DESA_RBTREE_HPP_
