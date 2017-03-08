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
 * Declaration: class with_csa<TI, AF>
 ************************************************/

template <typename TextIndex, typename AccessorFactory>
class with_csa {
 public:  // Public Type(s)
    using host_type = TextIndex;
    using size_type = text_index_trait::size_type;
    using value_type = text_index_trait::size_type;
    using term_type = text_index_trait::term_type;

 private:  // Private Types(s)
    using accessor_factory = AccessorFactory;
    using event = text_index_trait::event;

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
    template <typename Event>
    void update(Event const &);

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
};  // class with_csa<TI, AF>

/************************************************
 * Implementation: class with_csa<TI, AF>
 ************************************************/

template <typename TI, typename AF>
typename with_csa<TI, AF>::value_type with_csa<TI, AF>::at(size_type i) const {
    size_type off = 0;
    while (!sa_samples_[i]) {
        i = accessor_factory::template create<host_type>().to_host(this)->lf(i);
        ++off;
    }

    auto r = sa_samples_.rank(i, true);
    auto j = pi_.at(r - 1);

    auto sa = isa_samples_.select(j, true) + off;
    auto n = accessor_factory::template create<host_type>().to_host(this)->num_terms();
    return sa < n ? sa : sa - n;
}

template <typename TI, typename AF>
typename with_csa<TI, AF>::size_type with_csa<TI, AF>::rank(value_type j) const {
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
            v = accessor_factory::template create<host_type>().to_host(this)->lf(v);
        }

        return v;
    }
}

template <typename TI, typename AF>
inline typename with_csa<TI, AF>::term_type with_csa<TI, AF>::term(value_type j) const {
    auto const &wm = accessor_factory::template create<host_type>().get_wm(this);
    return wm.search(rank(j) + 1);
}

template <typename TI, typename AF>
inline typename with_csa<TI, AF>::value_type with_csa<TI, AF>::operator[](size_type i) const {
    return at(i);
}

template <typename TI, typename AF>
template <typename Sequence>
inline void with_csa<TI, AF>::update(
        typename event::template after_inserting_first_term<Sequence> const &) {
    insert_term(0, true);
    pi_.insert(0, 0);
}

template <typename TI, typename AF>
template <typename Sequence>
inline void with_csa<TI, AF>::update(
        typename event::template after_inserting_term<Sequence> const &info) {
    insert_term(info.pos, false);
}

template <typename TI, typename AF>
template <typename Sequence>
inline void with_csa<TI, AF>::update(
        typename event::template after_inserting_sequence<Sequence> const &) {
    add_samples(0);
}

template <typename TI, typename AF>
template <typename Event>
inline void with_csa<TI, AF>::update(Event const &) {
    // do nothing
}

template <typename TI, typename AF>
inline void with_csa<TI, AF>::insert_term(size_type i, bool is_sampled) {
    sa_samples_.insert(i, is_sampled);
    isa_samples_.insert(0, is_sampled);
}

template <typename TI, typename AF>
void with_csa<TI, AF>::add_samples(value_type j) {
    auto n = accessor_factory::template create<host_type>().to_host(this)->num_terms();
    if (j + 1 == n) { return; }

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
