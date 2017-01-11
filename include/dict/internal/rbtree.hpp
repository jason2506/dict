/************************************************
 *  rbtree.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_INTERNAL_RBTREE_HPP_
#define DICT_INTERNAL_RBTREE_HPP_

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace dict {

namespace internal {

/************************************************
 * Declaration: class rbtree<T, U>
 ************************************************/

template <typename T, typename Updater>
class rbtree {
 private:  // Private Type(s) - Part 1
    template <bool IsConst> class tree_iterator;

 public:  // Public Type(s)
    using size_type = std::size_t;
    using value_type = T;

    using iterator = tree_iterator<false>;
    using const_iterator = tree_iterator<true>;

 public:  // Public Method(s)
    rbtree();

    iterator root();
    const_iterator root() const;
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    iterator insert_before(iterator pos, value_type const &data, Updater const &update = Updater());
    iterator erase(iterator pos, Updater const &update = Updater());

    size_type size() const;

 private:  // Private Type(s) - Part 2
    enum class color: bool {red, black};
    class node;
    class node_pool;
    struct node_deleter;

    using weak_node_ptr = node *;
    using weak_const_node_ptr = node const *;
    using node_ptr = std::unique_ptr<node, node_deleter>;

 private:  // Private Method(s)
    void rebalance_after_insertion(weak_node_ptr ptr, weak_node_ptr parent, Updater const &update);
    void rebalance_after_erasure(weak_node_ptr ptr, weak_node_ptr parent, Updater const &update);

    void rotate_left(weak_node_ptr weak_ptr, Updater const &update);
    void rotate_right(weak_node_ptr weak_ptr, Updater const &update);

    weak_node_ptr next_node(weak_node_ptr ptr) const;
    weak_node_ptr prev_node(weak_node_ptr ptr) const;

    static bool is_red_node(weak_const_node_ptr ptr);
    static bool is_black_node(weak_const_node_ptr ptr);

 private:  // Private Static Property(ies)
    constexpr static size_type INITIAL_POOL_SIZE = 1024;
    static node_pool pool_;

 private:  // Private Property(ies)
    node_ptr root_;
    weak_node_ptr first_, last_;
};  // class rbtree<T, U>

/************************************************
 * Declaration: class rbtree<T, U>::node
 ************************************************/

template <typename T, typename U>
class rbtree<T, U>::node {
 public:  // Public Method(s)
    explicit node(value_type const &data);

    weak_const_node_ptr get_parent() const;
    weak_node_ptr get_parent();
    void set_parent(weak_node_ptr parent);

    weak_const_node_ptr get_left() const;
    weak_node_ptr get_left();
    void set_left(weak_node_ptr left);
    void set_left(node_ptr &&left);
    node_ptr move_left();

    weak_const_node_ptr get_right() const;
    weak_node_ptr get_right();
    void set_right(weak_node_ptr right);
    void set_right(node_ptr &&right);
    node_ptr move_right();

    color get_color() const;
    void set_color(color c);

    value_type const &data() const;
    value_type &data();

    size_type size() const;

 private:  // Private Property(ies)
    weak_node_ptr parent_;
    node_ptr left_, right_;
    color color_;
    value_type data_;
};  // class rbtree<T, U>::node

/************************************************
 * Declaration: class rbtree<T, U>::node_pool
 ************************************************/

template <typename T, typename U>
class rbtree<T, U>::node_pool {
 public:  // Public Method(s)
    explicit node_pool(size_type size);
    node_pool(node_pool const &) = delete;
    ~node_pool();

    node_pool &operator=(node_pool const &) = delete;

    weak_node_ptr new_node(value_type const &data);
    void delete_node(weak_node_ptr p);

 private:  // Private Type(s)
    union block {
        block()     { /* do nothing */ }
        ~block()    { /* do nothing */ }

        node data;
        block *next;
    };

 private:  // Private Property(ies)
    size_type next_size_;
    block *chunks_head_;
    block *free_list_head_;
};  // class rbtree<T, U>::node_pool

/************************************************
 * Declaration: struct rbtree<T, U>::node_deleter
 ************************************************/

template <typename T, typename U>
struct rbtree<T, U>::node_deleter {
    void operator()(weak_node_ptr p) const {
        pool_.delete_node(p);
    }
};  // struct rbtree<T, U>::node_deleter

/************************************************
 * Declaration: class rbtree<T, U>::tree_iterator<B>
 ************************************************/

template <typename T, typename U>
template <bool IsConst>
class rbtree<T, U>::tree_iterator {
 public:  // Public Type(s)
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename std::conditional<
        IsConst,
        rbtree<T, U>::value_type const,
        rbtree<T, U>::value_type
    >::type;
    using pointer = value_type *;
    using reference = value_type &;
    using difference_type = ptrdiff_t;

    using node_ptr = typename std::conditional<
        IsConst,
        weak_const_node_ptr,
        weak_node_ptr
    >::type;

 public:  // Public Method(s)
    tree_iterator();
    explicit tree_iterator(rbtree const *tree);
    tree_iterator(rbtree const *tree, node_ptr ptr);
    tree_iterator(rbtree const *tree, typename rbtree<T, U>::node_ptr const &ptr);

    bool has_parent();
    bool has_left();
    bool has_right();

    void go_parent();
    void go_left();
    void go_right();

    tree_iterator parent();
    tree_iterator left();
    tree_iterator right();

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

    node_ptr get_node_ptr();

 private:  // Private Property(ies)
    rbtree const *tree_;
    node_ptr ptr_;
};  // class rbtree<T, U>::tree_iterator<B>

/************************************************
 * Implementation: class rbtree<T, U>
 ************************************************/

template <typename T, typename U>
typename rbtree<T, U>::node_pool rbtree<T, U>::pool_(INITIAL_POOL_SIZE);

template <typename T, typename U>
inline rbtree<T, U>::rbtree() : first_(nullptr), last_(nullptr) {
    // do nothing
}

template <typename T, typename U>
inline typename rbtree<T, U>::iterator rbtree<T, U>::root() {
    return iterator(this, root_);
}

template <typename T, typename U>
inline typename rbtree<T, U>::const_iterator rbtree<T, U>::root() const {
    return const_iterator(this, root_);
}

template <typename T, typename U>
inline typename rbtree<T, U>::iterator rbtree<T, U>::begin() {
    return iterator(this, first_);
}

template <typename T, typename U>
inline typename rbtree<T, U>::const_iterator rbtree<T, U>::begin() const {
    return cbegin();
}

template <typename T, typename U>
inline typename rbtree<T, U>::const_iterator rbtree<T, U>::cbegin() const {
    return const_iterator(this, first_);
}

template <typename T, typename U>
inline typename rbtree<T, U>::iterator rbtree<T, U>::end() {
    return iterator(this);
}

template <typename T, typename U>
inline typename rbtree<T, U>::const_iterator rbtree<T, U>::end() const {
    return cend();
}

template <typename T, typename U>
inline typename rbtree<T, U>::const_iterator rbtree<T, U>::cend() const {
    return const_iterator(this);
}

template <typename T, typename U>
typename rbtree<T, U>::iterator rbtree<T, U>::insert_before(
        iterator pos, value_type const &data, U const &update) {
    auto new_node = pool_.new_node(data);
    auto ptr = pos.get_node_ptr();
    if (ptr) {
        if (ptr->get_left()) {
            ptr = ptr->get_left();
            while (ptr->get_right()) {
                ptr = ptr->get_right();
            }

            ptr->set_right(new_node);
        } else {
            if (ptr == first_) { first_ = new_node; }

            ptr->set_left(new_node);
        }

        new_node->set_parent(ptr);
        new_node->set_color(color::red);
    } else if (last_) {
        // insert after the last node
        new_node->set_parent(last_);
        new_node->set_color(color::red);
        last_->set_right(new_node);
        last_ = new_node;
    } else {
        // current node is at the root of the tree
        first_ = last_ = new_node;
        root_.reset(new_node);
        root_->set_color(color::black);
        return iterator(this, root_);
    }

    rebalance_after_insertion(new_node, new_node->get_parent(), update);
    return iterator(this, new_node);
}

template <typename T, typename U>
typename rbtree<T, U>::iterator rbtree<T, U>::erase(iterator pos, U const &update) {
    auto ptr = pos.get_node_ptr();
    if (first_ == ptr && last_ == ptr) {
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
    if (ptr->get_left() && ptr->get_right()) {
        // current node has two non-null children
        ptr_color = next_ptr->get_color();
        next_ptr->set_color(ptr->get_color());

        next_ptr->set_left(ptr->move_left());
        next_ptr->get_left()->set_parent(next_ptr);

        auto next_ptr_parent = next_ptr->get_parent();
        next_ptr->set_parent(parent);
        child = next_ptr->get_right();
        if (ptr == next_ptr_parent) {
            if (!parent) {
                root_ = ptr->move_right();
            } else if (ptr == parent->get_left()) {
                parent->set_left(ptr->move_right());
            } else {
                parent->set_right(ptr->move_right());
            }

            parent = next_ptr;
        } else {
            auto child_uptr = next_ptr->move_right();
            next_ptr->set_right(ptr->move_right());
            next_ptr->get_right()->set_parent(next_ptr);
            if (!parent) {
                root_ = next_ptr_parent->move_left();
            } else if (ptr == parent->get_left()) {
                parent->set_left(next_ptr_parent->move_left());
            } else {
                parent->set_right(next_ptr_parent->move_left());
            }

            next_ptr_parent->set_left(std::move(child_uptr));
            parent = next_ptr_parent;
        }
    } else {
        // current node has at most one non-null child
        node_ptr child_uptr;
        if (!ptr->get_left()) {
            child_uptr = ptr->move_right();
        } else {
            child_uptr = ptr->move_left();
        }

        // replace current node with its child
        child = child_uptr.get();
        if (!parent) {
            root_ = std::move(child_uptr);
        } else if (ptr == parent->get_left()) {
            parent->set_left(std::move(child_uptr));
        } else {
            parent->set_right(std::move(child_uptr));
        }
    }

    if (child) {
        child->set_parent(parent);
    }

    if (ptr == next_parent_it.get_node_ptr()) {
        update(pos);
    } else if (pos && pos.parent() != next_parent_it) {
        update(next_parent_it);
    } else {
        update(parent_it);
    }

    if (ptr_color == color::black) {
        rebalance_after_erasure(child, parent, update);
    }

    return iterator(this, next_ptr);
}

template <typename T, typename U>
void rbtree<T, U>::rebalance_after_insertion(
        weak_node_ptr ptr, weak_node_ptr parent, U const &update) {
    while (is_red_node(parent)) {
        auto grandparent = parent->get_parent();
        auto uncle = grandparent
            ? (parent == grandparent->get_left())
                ? grandparent->get_right()
                : grandparent->get_left()
            : nullptr;
        if (is_red_node(uncle)) {
            // Case 1
            parent->set_color(color::black);
            uncle->set_color(color::black);
            grandparent->set_color(color::red);
            ptr = grandparent;
            parent = ptr->get_parent();
        } else {
            // Case 2
            if (ptr == parent->get_right() &&
                parent == grandparent->get_left()) {
                rotate_left(parent, update);
                ptr = ptr->get_left();
                parent = ptr->get_parent();
            } else if (ptr == parent->get_left() &&
                     parent == grandparent->get_right()) {
                rotate_right(parent, update);
                ptr = ptr->get_right();
                parent = ptr->get_parent();
            }

            // Case 3
            parent->set_color(color::black);
            grandparent->set_color(color::red);
            if (ptr == parent->get_left()) {
                rotate_right(grandparent, update);
            } else {
                rotate_left(grandparent, update);
            }
        }
    }

    root_->set_color(color::black);
}

template <typename T, typename U>
void rbtree<T, U>::rebalance_after_erasure(
        weak_node_ptr ptr, weak_node_ptr parent, U const &update) {
    while (parent && is_black_node(ptr)) {
        auto sibling = (ptr == parent->get_left())
            ? parent->get_right()
            : parent->get_left();
        if (sibling->get_color() == color::red) {
            // Case 1
            sibling->set_color(color::black);
            parent->set_color(color::red);
            if (ptr == parent->get_left()) {
                rotate_left(parent, update);
                sibling = parent->get_right();
            } else {
                rotate_right(parent, update);
                sibling = parent->get_left();
            }
        }

        if (is_black_node(sibling->get_left()) &&
            is_black_node(sibling->get_right())) {
            // Case 2
            sibling->set_color(color::red);
            ptr = parent;
            parent = parent->get_parent();
        } else if (ptr == parent->get_left()) {
            if (is_black_node(sibling->get_right())) {
                // Case 3
                if (sibling->get_left()) {
                    sibling->get_left()->set_color(color::black);
                }

                sibling->set_color(color::red);
                rotate_right(sibling, update);
                sibling = parent->get_right();
            }

            // Case 4
            sibling->set_color(parent->get_color());
            parent->set_color(color::black);
            sibling->get_right()->set_color(color::black);
            rotate_left(parent, update);
            ptr = root_.get();
            break;
        } else {
            if (is_black_node(sibling->get_left())) {
                // Case 3
                if (sibling->get_right()) {
                    sibling->get_right()->set_color(color::black);
                }

                sibling->set_color(color::red);
                rotate_left(sibling, update);
                sibling = parent->get_left();
            }

            // Case 4
            sibling->set_color(parent->get_color());
            parent->set_color(color::black);
            sibling->get_left()->set_color(color::black);
            rotate_right(parent, update);
            ptr = root_.get();
            break;
        }
    }

    if (ptr) {
        ptr->set_color(color::black);
    }
}

template <typename T, typename U>
void rbtree<T, U>::rotate_left(weak_node_ptr ptr, U const &update) {
    auto parent = ptr->get_parent();
    auto right = ptr->move_right();
    right->set_parent(parent);
    ptr->set_parent(right.get());

    ptr->set_right(right->move_left());
    if (!parent) {
        right->set_left(std::move(root_));
        root_ = std::move(right);
    } else if (ptr == parent->get_left()) {
        right->set_left(parent->move_left());
        parent->set_left(std::move(right));
    } else {
        right->set_left(parent->move_right());
        parent->set_right(std::move(right));
    }

    if (ptr->get_right()) {
        ptr->get_right()->set_parent(ptr);
    }

    update(iterator(this, ptr));
}

template <typename T, typename U>
void rbtree<T, U>::rotate_right(weak_node_ptr ptr, U const &update) {
    auto parent = ptr->get_parent();
    auto left = ptr->move_left();
    left->set_parent(parent);
    ptr->set_parent(left.get());

    ptr->set_left(left->move_right());
    if (!parent) {
        left->set_right(std::move(root_));
        root_ = std::move(left);
    } else if (ptr == parent->get_left()) {
        left->set_right(parent->move_left());
        parent->set_left(std::move(left));
    } else {
        left->set_right(parent->move_right());
        parent->set_right(std::move(left));
    }

    if (ptr->get_left()) {
        ptr->get_left()->set_parent(ptr);
    }

    update(iterator(this, ptr));
}

template <typename T, typename U>
typename rbtree<T, U>::weak_node_ptr rbtree<T, U>::next_node(weak_node_ptr ptr) const {
    if (!ptr) {
        ptr = first_;
    } else if (ptr == last_) {
        ptr = nullptr;
    } else if (ptr->get_right()) {
        ptr = ptr->get_right();
        while (ptr->get_left()) {
            ptr = ptr->get_left();
        }
    } else {
        while (ptr == ptr->get_parent()->get_right()) {
            ptr = ptr->get_parent();
        }

        ptr = ptr->get_parent();
    }

    return ptr;
}

template <typename T, typename U>
typename rbtree<T, U>::weak_node_ptr rbtree<T, U>::prev_node(weak_node_ptr ptr) const {
    if (!ptr) {
        ptr = last_;
    } else if (ptr == first_) {
        ptr = nullptr;
    } else if (ptr->get_left()) {
        ptr = ptr->get_left();
        while (ptr->get_right()) {
            ptr = ptr->get_right();
        }
    } else {
        while (ptr == ptr->get_parent()->get_left()) {
            ptr = ptr->get_parent();
        }

        ptr = ptr->get_parent();
    }

    return ptr;
}

template <typename T, typename U>
inline bool rbtree<T, U>::is_red_node(weak_const_node_ptr ptr) {
    return ptr && ptr->get_color() == color::red;
}

template <typename T, typename U>
inline bool rbtree<T, U>::is_black_node(weak_const_node_ptr ptr) {
    return !is_red_node(ptr);
}

template <typename T, typename U>
inline typename rbtree<T, U>::size_type rbtree<T, U>::size() const {
    return root_ ? root_->size() : 0;
}

/************************************************
 * Implementation: struct rbtree<T, U>::node
 ************************************************/

template <typename T, typename U>
inline rbtree<T, U>::node::node(T const &data)
    : parent_(nullptr), data_(data) {
    // do nothing
}

template <typename T, typename U>
inline typename rbtree<T, U>::weak_const_node_ptr rbtree<T, U>::node::get_parent() const {
    return parent_;
}

template <typename T, typename U>
inline typename rbtree<T, U>::weak_node_ptr rbtree<T, U>::node::get_parent() {
    return parent_;
}

template <typename T, typename U>
inline void rbtree<T, U>::node::set_parent(weak_node_ptr parent) {
    parent_ = parent;
}

template <typename T, typename U>
inline typename rbtree<T, U>::weak_const_node_ptr rbtree<T, U>::node::get_left() const {
    return left_.get();
}

template <typename T, typename U>
inline typename rbtree<T, U>::weak_node_ptr rbtree<T, U>::node::get_left() {
    return left_.get();
}

template <typename T, typename U>
inline void rbtree<T, U>::node::set_left(weak_node_ptr left) {
    left_.reset(left);
}

template <typename T, typename U>
inline void rbtree<T, U>::node::set_left(node_ptr &&left) {
    left_ = std::move(left);
}

template <typename T, typename U>
inline typename rbtree<T, U>::node_ptr rbtree<T, U>::node::move_left() {
    return std::move(left_);
}

template <typename T, typename U>
inline typename rbtree<T, U>::weak_const_node_ptr rbtree<T, U>::node::get_right() const {
    return right_.get();
}

template <typename T, typename U>
inline typename rbtree<T, U>::weak_node_ptr rbtree<T, U>::node::get_right() {
    return right_.get();
}

template <typename T, typename U>
inline void rbtree<T, U>::node::set_right(weak_node_ptr right) {
    right_.reset(right);
}

template <typename T, typename U>
inline void rbtree<T, U>::node::set_right(node_ptr &&right) {
    right_ = std::move(right);
}

template <typename T, typename U>
inline typename rbtree<T, U>::node_ptr rbtree<T, U>::node::move_right() {
    return std::move(right_);
}

template <typename T, typename U>
inline typename rbtree<T, U>::color rbtree<T, U>::node::get_color() const {
    return color_;
}

template <typename T, typename U>
inline void rbtree<T, U>::node::set_color(color c) {
    color_ = c;
}

template <typename T, typename U>
inline T const &rbtree<T, U>::node::data() const {
    return data_;
}

template <typename T, typename U>
inline T &rbtree<T, U>::node::data() {
    return data_;
}

template <typename T, typename U>
inline typename rbtree<T, U>::size_type rbtree<T, U>::node::size() const {
    return 1
        + (left_ ? left_->size() : 0)
        + (right_ ? right_->size() : 0);
}

/************************************************
 * Implementation: class rbtree<T, U>::node_pool
 ************************************************/

template <typename T, typename U>
inline rbtree<T, U>::node_pool::node_pool(size_type size)
    : next_size_(size), chunks_head_(nullptr), free_list_head_(nullptr) {
    // do nothing
}

template <typename T, typename U>
inline rbtree<T, U>::node_pool::~node_pool() {
    auto chunk = chunks_head_;
    while (chunk != nullptr) {
        auto next_chunk = chunk[0].next;
        delete [] chunk;
        chunk = next_chunk;
    }
}

template <typename T, typename U>
typename rbtree<T, U>::weak_node_ptr rbtree<T, U>::node_pool::new_node(value_type const &data) {
    if (free_list_head_ == nullptr) {
        auto chunk = new block[next_size_ + 1];
        chunk[0].next = chunks_head_;
        chunks_head_ = chunk;
        free_list_head_ = chunk + 1;
        for (size_type i = 1; i < next_size_; i++) {
            chunk[i].next = &chunk[i + 1];
        }

        chunk[next_size_].next = nullptr;
        next_size_ <<= 1;
    }

    auto p = &(free_list_head_->data);
    free_list_head_ = free_list_head_->next;
    return new(p) node(data);
}

template <typename T, typename U>
inline void rbtree<T, U>::node_pool::delete_node(weak_node_ptr p) {
    p->~node();
    reinterpret_cast<block *>(p)->next = free_list_head_;
    free_list_head_ = reinterpret_cast<block *>(p);
}

/************************************************
 * Implementation: class rbtree<T, U>::tree_iterator<B>
 ************************************************/

template <typename T, typename U>
template <bool B>
inline rbtree<T, U>::tree_iterator<B>::tree_iterator()
    : tree_(nullptr), ptr_(nullptr) {
    // do nothing
}

template <typename T, typename U>
template <bool B>
inline rbtree<T, U>::tree_iterator<B>::tree_iterator(rbtree const *tree)
    : tree_(tree), ptr_(nullptr) {
    // do nothing
}

template <typename T, typename U>
template <bool B>
inline rbtree<T, U>::tree_iterator<B>::tree_iterator(rbtree const *tree, node_ptr ptr)
    : tree_(tree), ptr_(ptr) {
    // do nothing
}

template <typename T, typename U>
template <bool B>
inline rbtree<T, U>::tree_iterator<B>::tree_iterator(
        rbtree const *tree, typename rbtree<T, U>::node_ptr const &ptr)
    : tree_(tree), ptr_(ptr.get()) {
    // do nothing
}

template <typename T, typename U>
template <bool B>
inline bool rbtree<T, U>::tree_iterator<B>::has_parent() {
    return ptr_->get_parent();
}

template <typename T, typename U>
template <bool B>
inline bool rbtree<T, U>::tree_iterator<B>::has_left() {
    return ptr_->get_left();
}

template <typename T, typename U>
template <bool B>
inline bool rbtree<T, U>::tree_iterator<B>::has_right() {
    return ptr_->get_right();
}

template <typename T, typename U>
template <bool B>
inline void rbtree<T, U>::tree_iterator<B>::go_parent() {
    ptr_ = ptr_->get_parent();
}

template <typename T, typename U>
template <bool B>
inline void rbtree<T, U>::tree_iterator<B>::go_left() {
    ptr_ = ptr_->get_left();
}

template <typename T, typename U>
template <bool B>
inline void rbtree<T, U>::tree_iterator<B>::go_right() {
    ptr_ = ptr_->get_right();
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<B> rbtree<T, U>::tree_iterator<B>::parent() {
    return tree_iterator(tree_, ptr_->get_parent());
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<B> rbtree<T, U>::tree_iterator<B>::left() {
    return tree_iterator(tree_, ptr_->get_left());
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<B> rbtree<T, U>::tree_iterator<B>::right() {
    return tree_iterator(tree_, ptr_->get_right());
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<false>
rbtree<T, U>::tree_iterator<B>::unconst() const {
    using node_ptr = typename tree_iterator<false>::node_ptr;
    return tree_iterator<false>(tree_, const_cast<node_ptr>(ptr_));
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<B> &rbtree<T, U>::tree_iterator<B>::operator++() {
    ptr_ = tree_->next_node(ptr_);
    return *this;
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<B> rbtree<T, U>::tree_iterator<B>::operator++(int) {
    tree_iterator it(tree_, ptr_);
    operator++();
    return it;
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<B> &rbtree<T, U>::tree_iterator<B>::operator--() {
    ptr_ = tree_->prev_node(ptr_);
    return *this;
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<B> rbtree<T, U>::tree_iterator<B>::operator--(int) {
    tree_iterator it(tree_, ptr_);
    operator--();
    return it;
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<B>::reference
rbtree<T, U>::tree_iterator<B>::operator*() {
    return *operator->();
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<B>::pointer
rbtree<T, U>::tree_iterator<B>::operator->() {
    return &ptr_->data();
}

template <typename T, typename U>
template <bool B>
inline bool rbtree<T, U>::tree_iterator<B>::operator==(tree_iterator it) const {
    return ptr_ == it.ptr_;
}

template <typename T, typename U>
template <bool B>
inline bool rbtree<T, U>::tree_iterator<B>::operator!=(tree_iterator it) const {
    return !(*this == it);
}

template <typename T, typename U>
template <bool B>
inline bool rbtree<T, U>::tree_iterator<B>::operator!() const {
    return !this->operator bool();
}

template <typename T, typename U>
template <bool B>
inline rbtree<T, U>::tree_iterator<B>::operator bool() const {
    return static_cast<bool>(ptr_);
}

template <typename T, typename U>
template <bool B>
inline typename rbtree<T, U>::template tree_iterator<B>::node_ptr
rbtree<T, U>::tree_iterator<B>::get_node_ptr() {
    return ptr_;
}

}  // namespace internal

}  // namespace dict

#endif  // DICT_INTERNAL_RBTREE_HPP_
