/************************************************
 *  bit_vector.hpp
 *  DESA
 *
 *  Copyright (c) 2015-2016, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_INTERNAL_BIT_VECTOR_HPP_
#define DESA_INTERNAL_BIT_VECTOR_HPP_

#include <bitset>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "rbtree.hpp"

namespace desa {

namespace internal {

/************************************************
 * Declaration: class bit_vector<N>
 ************************************************/

template <::std::size_t N>
class bit_vector {
 public:  // Public Type(s)
    using size_type = ::std::size_t;
    using value_type = bool;

 private:  // Private Static Property(ies)
    static constexpr size_type MAX_BLOCK_SIZE = 2 * N;
    static constexpr size_type MIN_BLOCK_SIZE = 0.25 * MAX_BLOCK_SIZE;
    static constexpr size_type MAX_MERGE_SIZE =
        MAX_BLOCK_SIZE - 1 < 0.9 * MAX_BLOCK_SIZE
            ? MAX_BLOCK_SIZE - 1
            : 0.9 * MAX_BLOCK_SIZE;

 private:  // Private Type(s) - Part 1
    using bitset = ::std::bitset<MAX_BLOCK_SIZE>;

 public:  // Public Method(s)
    ~bit_vector();

    bit_vector &set(size_type i, value_type b = true);
    bit_vector &reset(size_type i);
    void insert(size_type i, value_type b);
    value_type erase(size_type i);

    ::std::pair<value_type, size_type> access_and_rank(size_type i) const;
    ::std::pair<value_type, size_type> access_and_rank(size_type i, value_type b) const;
    size_type rank(size_type i, value_type b) const;
    size_type select(size_type i, value_type b) const;
    size_type count() const;
    size_type size() const;

    value_type operator[](size_type i) const;

 private:  // Private Type(s) - Part 2
    struct block;
    using bstree = rbtree<block>;

 private:  // Private Static Method(s)
    static void equalize_blocks(block &p, block &q);    // NOLINT(runtime/references)
    static void merge_blocks(block &p, block &q);       // NOLINT(runtime/references)
    static void update_counters(typename bstree::iterator it);

 private:  // Private Method(s)
    // NOLINTNEXTLINE(runtime/references)
    typename bstree::iterator find_block(size_type i, size_type &pos, size_type &rank);
    // NOLINTNEXTLINE(runtime/references)
    typename bstree::const_iterator find_block(size_type i, size_type &pos, size_type &rank) const;
    // NOLINTNEXTLINE(runtime/references)
    typename bstree::const_iterator find_bit(size_type &i, size_type &pos, size_type &rank) const;

 private:  // Private Property(ies)
    bstree tree_;
};  // class bit_vector<N>

/************************************************
 * Declaration: struct bit_vector<N>::block
 ************************************************/

template <::std::size_t N>
struct bit_vector<N>::block {
    block();

    size_type num_bits;
    size_type num_sub_bits;
    size_type num_sub_set_bits;
    bitset bits;
};  // class bit_vector<N>::block

/************************************************
 * Implementation: class bit_vector<N>
 ************************************************/

template <::std::size_t N>
inline bit_vector<N>::~bit_vector() {
    // do nothing
}

template <::std::size_t N>
inline bit_vector<N> &bit_vector<N>::set(size_type i, value_type b) {
    size_type pos = 0, rank = 0;
    auto it = find_block(i, pos, rank);
    i -= pos;

    it->bits[i] = b;
    update_counters(it);
    return *this;
}

template <::std::size_t N>
inline bit_vector<N> &bit_vector<N>::reset(size_type i) {
    return set(i, false);
}

template <::std::size_t N>
void bit_vector<N>::insert(size_type i, value_type b) {
    if (!tree_.root()) {
        block bb;
        bb.num_bits = bb.num_sub_bits = 1;
        bb.num_sub_set_bits = (b ? 1 : 0);
        bb.bits[i] = b;
        tree_.insert_before(tree_.end(), bb, update_counters);
        return;
    }

    auto it = tree_.end();
    if (i == size()) {
        --it;
        i = it->num_bits;
    } else {
        size_type pos = 0, rank = 0;
        it = find_block(i, pos, rank);
        i -= pos;
    }

    if (it->num_bits == MAX_BLOCK_SIZE) {
        auto prev_it = ::std::prev(it);
        auto next_it = ::std::next(it);
        if (prev_it &&
                prev_it->num_bits + it->num_bits <= (MAX_MERGE_SIZE << 1) &&
                (!next_it || prev_it->num_bits < next_it->num_bits)) {
            i += prev_it->num_bits;
            equalize_blocks(*prev_it, *it);
            update_counters(prev_it);
            update_counters(it);

            if (i >= prev_it->num_bits) {
                i -= prev_it->num_bits;
            } else {
                it = prev_it;
            }
        } else if (next_it &&
                next_it->num_bits + it->num_bits <= (MAX_MERGE_SIZE << 1)) {
            equalize_blocks(*it, *next_it);
            update_counters(it);
            update_counters(next_it);

            if (i >= it->num_bits) {
                i -= it->num_bits;
                it = next_it;
            }
        } else {
            block bb;
            equalize_blocks(bb, *it);
            update_counters(it);
            auto new_it = tree_.insert_before(it, bb, update_counters);
            update_counters(new_it);

            if (i >= new_it->num_bits) {
                i -= new_it->num_bits;
            } else {
                it = new_it;
            }
        }
    }

    // insert bit in the vector
    if (i == 0) {
        it->bits <<= 1;
    } else if (i < it->num_bits) {
        auto mask = (it->bits >> i) << i;
        it->bits ^= mask;
        it->bits |= mask << 1;
    }

    it->bits[i] = b;

    // update counters
    it->num_bits++;
    update_counters(it);
}

template <::std::size_t N>
typename bit_vector<N>::value_type bit_vector<N>::erase(size_type i) {
    size_type pos = 0, rank = 0;
    auto it = find_block(i, pos, rank);
    i -= pos;

    // erase bit in the vector
    value_type b = it->bits[i];
    if (i == 0) {
        it->bits >>= 1;
    } else if (i < it->num_bits - 1) {
        it->bits[i] = 0;

        auto mask = (it->bits >> i) << i;
        it->bits ^= mask;
        it->bits |= mask >> 1;
    }

    // update counters
    it->num_bits--;
    update_counters(it);

    // delete or merge small blocks
    if (it->num_bits == 0) {
        tree_.erase(it, update_counters);
    } else if (it->num_bits < MIN_BLOCK_SIZE) {
        auto prev_it = ::std::prev(it);
        auto next_it = ::std::next(it);
        if (prev_it && (!next_it || prev_it->num_bits < next_it->num_bits)) {
            if (prev_it->num_bits + it->num_bits <= MAX_MERGE_SIZE) {
                merge_blocks(*prev_it, *it);
                update_counters(it);
                tree_.erase(it, update_counters);
            } else if (prev_it->num_bits + it->num_bits <= (MAX_MERGE_SIZE << 1)) {
                equalize_blocks(*prev_it, *it);
                update_counters(it);
                update_counters(next_it);
            }
        } else if (next_it) {
            if (next_it->num_bits + it->num_bits <= MAX_MERGE_SIZE) {
               merge_blocks(*it, *next_it);
               update_counters(it);
               tree_.erase(next_it, update_counters);
            } else if (next_it->num_bits + it->num_bits <= (MAX_MERGE_SIZE << 1)) {
               equalize_blocks(*it, *next_it);
               update_counters(it);
               update_counters(next_it);
            }
        }
    }

    return b;
}

template <::std::size_t N>
inline ::std::pair<typename bit_vector<N>::value_type, typename bit_vector<N>::size_type>
bit_vector<N>::access_and_rank(size_type i) const {
    size_type pos = 0, rank = 0;
    auto it = find_bit(i, pos, rank);
    auto b = it->bits[i];
    auto r = b ? rank : i + pos + 1 - rank;
    return ::std::make_pair(b, r);
}

template <::std::size_t N>
inline ::std::pair<typename bit_vector<N>::value_type, typename bit_vector<N>::size_type>
    bit_vector<N>::access_and_rank(size_type i, value_type b) const {
    size_type pos = 0, rank = 0;
    auto it = find_bit(i, pos, rank);
    auto r = b ? rank : i + pos + 1 - rank;
    return ::std::make_pair(it->bits[i], r);
}

template <::std::size_t N>
inline typename bit_vector<N>::size_type bit_vector<N>::rank(size_type i, value_type b) const {
    return access_and_rank(i, b).second;
}

template <::std::size_t N>
typename bit_vector<N>::size_type bit_vector<N>::select(size_type i, value_type b) const {
    size_type pos = 0;

    auto it = tree_.root();
    while (it) {
        size_type num_left_bits, num_left_set_bits;
        if (it.has_left()) {
            num_left_bits = it.left()->num_sub_bits;
            num_left_set_bits = b
                ? it.left()->num_sub_set_bits
                : num_left_bits - it.left()->num_sub_set_bits;
        } else {
            num_left_bits = num_left_set_bits = 0;
        }

        if (i < num_left_set_bits) {
            it.go_left();
        } else {
            pos += num_left_bits;
            i -= num_left_set_bits;

            auto num_set_bits = b
                ? it->bits.count()
                : it->num_bits - it->bits.count();
            if (i < num_set_bits) {
                break;
            } else {
                pos += it->num_bits;
                i -= num_set_bits;
                it.go_right();
            }
        }
    }

    for (decltype(i) j = 0, k = 0; k <= i; ++pos, ++j) {
        if (it->bits[j] == b) { ++k; }
    }

    return pos - 1;
}

template <::std::size_t N>
inline typename bit_vector<N>::size_type bit_vector<N>::count() const {
    auto root = tree_.root();
    return root ? root->num_sub_set_bits : 0;
}

template <::std::size_t N>
inline typename bit_vector<N>::size_type bit_vector<N>::size() const {
    auto root = tree_.root();
    return root ? tree_.root()->num_sub_bits : 0;
}

template <::std::size_t N>
inline typename bit_vector<N>::value_type bit_vector<N>::operator[](size_type i) const {
    size_type pos = 0, rank = 0;
    auto it = find_block(i, pos, rank);
    return it->bits[i - pos];
}

template <::std::size_t N>
inline typename bit_vector<N>::bstree::iterator  // NOLINTNEXTLINE(runtime/references)
bit_vector<N>::find_block(size_type i, size_type &pos, size_type &rank) {
    auto const &that = *this;
    auto it = that.find_block(i, pos, rank);
    return it.unconst();
}

template <::std::size_t N>
typename bit_vector<N>::bstree::const_iterator  // NOLINTNEXTLINE(runtime/references)
bit_vector<N>::find_block(size_type i, size_type &pos, size_type &rank) const {
    auto it = tree_.root();
    while (it) {
        size_type num_left_bits, num_left_set_bits;
        if (it.has_left()) {
            num_left_bits = it.left()->num_sub_bits;
            num_left_set_bits = it.left()->num_sub_set_bits;
        } else {
            num_left_bits = num_left_set_bits = 0;
        }

        if (i < num_left_bits) {
            it.go_left();
        } else {
            pos += num_left_bits;
            rank += num_left_set_bits;

            i -= num_left_bits;
            if (i < it->num_bits) {
                break;
            } else {
                pos += it->num_bits;
                rank += it->bits.count();

                i -= it->num_bits;
                it.go_right();
            }
        }
    }

    return it;
}

template <::std::size_t N>
inline typename bit_vector<N>::bstree::const_iterator  // NOLINTNEXTLINE(runtime/references)
bit_vector<N>::find_bit(size_type &i, size_type &pos, size_type &rank) const {
    auto it = find_block(i, pos, rank);
    i -= pos;
    rank += (it->bits << (MAX_BLOCK_SIZE - i - 1)).count();
    return it;
}

template <::std::size_t N>  // NOLINTNEXTLINE(runtime/references)
void bit_vector<N>::equalize_blocks(block &p, block &q) {
    auto num_bits = (p.num_bits + q.num_bits) / 2;
    if (p.num_bits > q.num_bits) {
        auto offset = p.num_bits - num_bits;
        q.bits <<= offset;
        q.bits |= p.bits >> num_bits;
        p.bits ^= (p.bits >> num_bits) << num_bits;

        p.num_bits -= offset;
        q.num_bits += offset;
    } else {
        auto offset = q.num_bits - num_bits;
        auto mask = (q.bits >> offset) << offset;
        p.bits |= (q.bits ^ mask) << p.num_bits;
        q.bits >>= offset;

        p.num_bits += offset;
        q.num_bits -= offset;
    }
}

template <::std::size_t N>  // NOLINTNEXTLINE(runtime/references)
void bit_vector<N>::merge_blocks(block &p, block &q) {
    p.bits |= q.bits << p.num_bits;
    q.bits.reset();

    p.num_bits += q.num_bits;
    q.num_bits = 0;
}

template <::std::size_t N>
void bit_vector<N>::update_counters(typename bstree::iterator it) {
    while (it) {
        it->num_sub_bits = it->num_bits;
        it->num_sub_set_bits = it->bits.count();

        if (it.has_left()) {
            auto left = it.left();
            it->num_sub_bits += left->num_sub_bits;
            it->num_sub_set_bits += left->num_sub_set_bits;
        }

        if (it.has_right()) {
            auto right = it.right();
            it->num_sub_bits += right->num_sub_bits;
            it->num_sub_set_bits += right->num_sub_set_bits;
        }

        it.go_parent();
    }
}

/************************************************
 * Implementation: struct bit_vector<N>::block
 ************************************************/

template <::std::size_t N>
bit_vector<N>::block::block()
    : num_bits(0), num_sub_bits(0), num_sub_set_bits(0) {
    // do nothing
}

}  // namespace internal

}  // namespace desa

#endif  // DESA_INTERNAL_BIT_VECTOR_HPP_
