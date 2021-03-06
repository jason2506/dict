/************************************************
 *  with_csa.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_WITH_CSA_HPP_
#define DICT_WITH_CSA_HPP_

#include "internal/bit_vector.hpp"
#include "internal/permutation.hpp"

namespace dict {

/************************************************
 * Declaration: class with_csa<TI, T>
 ************************************************/

template <typename TextIndex, typename Trait>
class with_csa {
 public:  // Public Type(s)
    using host_type = TextIndex;
    using size_type = typename Trait::size_type;
    using value_type = typename Trait::size_type;
    using term_type = typename Trait::term_type;

 private:  // Private Types(s)
    using helper = typename Trait::helper;
    using event = typename Trait::event;

 public:  // Public Method(s)
    value_type at(size_type i) const;
    size_type rank(value_type j) const;
    term_type term(value_type j) const;

    value_type operator[](size_type i) const;

 protected:  // Protected Method(s)
    template <typename Sequence>
    void update(typename event::template after_inserting_first_term<Sequence> const &);
    template <typename Sequence>
    void update(typename event::template after_inserting_term<Sequence> const &info);
    template <typename Sequence>
    void update(typename event::template after_inserting_sequence<Sequence> const &);

    void update(typename event::before_erasuring_sequence const &info);
    void update(typename event::after_erasuring_term const &info);
    void update(typename event::after_moving_term const &info);
    void update(typename event::after_erasuring_sequence const &);

 private:  // Private Method(s)
    void insert_term(size_type i, bool is_sampled);
    void add_samples(value_type j);

 private:  // Private Static Property(ies)
    static constexpr size_type MAX_SAMPLE_DISTANCE = 100;
    static constexpr size_type BIT_BLOCK_SIZE = 64;

 private:  // Private Property(ies)
    internal::bit_vector<BIT_BLOCK_SIZE> isa_samples_;
    internal::bit_vector<BIT_BLOCK_SIZE> sa_samples_;
    internal::permutation pi_;

    size_type erased_isa_pos_;
};  // class with_csa<TI, T>

/************************************************
 * Implementation: class with_csa<TI, T>
 ************************************************/

template <typename TI, typename T>
typename with_csa<TI, T>::value_type with_csa<TI, T>::at(size_type i) const {
    size_type off = 0;
    while (!sa_samples_[i]) {
        i = helper::to_host(this)->lf(i);
        ++off;
    }

    auto r = sa_samples_.rank(i, true);
    auto j = pi_.at(r - 1);

    auto sa = isa_samples_.select(j, true) + off;
    auto n = helper::to_host(this)->num_terms();
    return sa < n ? sa : sa - n;
}

template <typename TI, typename T>
typename with_csa<TI, T>::size_type with_csa<TI, T>::rank(value_type j) const {
    auto br_pair = isa_samples_.access_and_rank(j, true);
    auto b = br_pair.first;
    auto r = br_pair.second;
    if (b) {
        // retrieve a value at a sampled position
        auto i = pi_.rank(r - 1);
        return sa_samples_.select(i, true);
    } else {
        // retrieve a value at a unsampled position
        auto off = isa_samples_.select(r, true) - j;
        auto i = pi_.rank(r);
        auto v = sa_samples_.select(i, true);
        for (decltype(off) t = 0; t < off; ++t) {
            v = helper::to_host(this)->lf(v);
        }

        return v;
    }
}

template <typename TI, typename T>
inline typename with_csa<TI, T>::term_type with_csa<TI, T>::term(value_type j) const {
    auto const &wm = helper::get_wm(this);
    return wm.search(rank(j) + 1);
}

template <typename TI, typename T>
inline typename with_csa<TI, T>::value_type with_csa<TI, T>::operator[](size_type i) const {
    return at(i);
}

template <typename TI, typename T>
template <typename Sequence>
inline void with_csa<TI, T>::update(
        typename event::template after_inserting_first_term<Sequence> const &) {
    insert_term(0, true);
    pi_.insert(0, 0);
}

template <typename TI, typename T>
template <typename Sequence>
inline void with_csa<TI, T>::update(
        typename event::template after_inserting_term<Sequence> const &info) {
    insert_term(info.pos, false);
}

template <typename TI, typename T>
template <typename Sequence>
inline void with_csa<TI, T>::update(
        typename event::template after_inserting_sequence<Sequence> const &) {
    add_samples(0);
}

template <typename TI, typename T>
inline void with_csa<TI, T>::update(typename event::before_erasuring_sequence const &info) {
    erased_isa_pos_ = at(info.pos);
}

template <typename TI, typename T>
inline void with_csa<TI, T>::update(typename event::after_erasuring_term const &info) {
    auto pos = info.pos;
    sa_samples_.erase(pos);
    auto b = isa_samples_.erase(erased_isa_pos_);
    if (b) {
        auto r = pos > 0 ? sa_samples_.rank(pos - 1, true) : 0;
        pi_.erase(r);
    }

    if (erased_isa_pos_) { --erased_isa_pos_; }
}

template <typename TI, typename T>
void with_csa<TI, T>::update(typename event::after_moving_term const &info) {
    auto from_pos = info.from_pos;
    auto to_pos = info.to_pos;

    auto b = sa_samples_.erase(from_pos);
    if (b) {
        auto from_rank = from_pos > 0 ? sa_samples_.rank(from_pos - 1, true) : 0;
        auto to_rank = to_pos > 0 ? sa_samples_.rank(to_pos - 1, true) : 0;
        if (from_rank != to_rank) {
            pi_.move(from_rank, to_rank);
        }
    }

    sa_samples_.insert(to_pos, b);
}

template <typename TI, typename T>
inline void with_csa<TI, T>::update(typename event::after_erasuring_sequence const &) {
    auto const &wm = helper::get_wm(this);
    if (wm.size()) {
        add_samples(erased_isa_pos_);
    }
}

template <typename TI, typename T>
inline void with_csa<TI, T>::insert_term(size_type i, bool is_sampled) {
    sa_samples_.insert(i, is_sampled);
    isa_samples_.insert(0, is_sampled);
}

template <typename TI, typename T>
void with_csa<TI, T>::add_samples(value_type j) {
    auto n = helper::to_host(this)->num_terms();
    if (j + 1 == n) {
        if (!isa_samples_[j]) {
            auto r = isa_samples_.rank(j, true);
            sa_samples_.set(0);
            isa_samples_.set(j);
            pi_.insert(0, r);
        }

        return;
    }

    auto r = isa_samples_.rank(j, true);
    auto left_sample_pos = r > 0 ? isa_samples_.select(r - 1, true) + 1 : 0;
    auto right_sample_pos = isa_samples_.select(r, true);

    auto p = right_sample_pos;
    while (p > left_sample_pos + MAX_SAMPLE_DISTANCE) {
        p -= MAX_SAMPLE_DISTANCE + 1;

        auto i = rank(p);
        auto k = i > 0 ? sa_samples_.rank(i - 1, true) : 0;
        sa_samples_.set(i);
        isa_samples_.set(p);
        pi_.insert(k, r);
    }
}

}  // namespace dict

#endif  // DICT_WITH_CSA_HPP_
