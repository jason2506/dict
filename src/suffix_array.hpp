/************************************************
 *  suffix_array.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_SUFFIX_ARRAY_HPP_
#define DESA_SUFFIX_ARRAY_HPP_

#include <cassert>

#include "bit_vector.hpp"
#include "permutation.hpp"
#include "wavelet_tree.hpp"
#include "idx_tree.hpp"

namespace desa
{

namespace impl
{

/************************************************
 * Declaration: class suffix_array
 ************************************************/

class suffix_array
{
public: // Public Type(s)
    typedef ::std::size_t size_type;
    typedef ::std::size_t value_type;
    typedef ::std::uint16_t term_type;

public: // Public Method(s)
    suffix_array(void);

    template <typename InputIterator>
    void insert(InputIterator begin, InputIterator end);

    size_type size(void) const;
    value_type at(size_type i) const;
    size_type rank(value_type j) const;
    size_type lcp(size_type i) const;
    term_type bwt(size_type i) const;
    term_type term(value_type j) const;

    size_type psi(size_type i) const;
    size_type lf(size_type i) const;

    value_type operator[](size_type i) const;

private: // Private Method(s)
    void insert_term(size_type i, term_type c, bool is_sampled);
    void add_samples(value_type j);
    void insert_lcp(size_type kp, size_type psi_kp, size_type lf_kp, size_type &lcp);

private: // Private Static Property(ies)
    static constexpr size_type MAX_SAMPLE_DISTANCE = 100;
    static constexpr size_type BIT_BLOCK_SIZE = 64;

private: // Private Property(ies)
    wavelet_tree<term_type> wt_;
    bit_vector<BIT_BLOCK_SIZE> isa_samples_;
    bit_vector<BIT_BLOCK_SIZE> sa_samples_;
    permutation pi_;
    idx_tree lcpa_;
    size_type sentinel_pos_;
    size_type sentinel_rank_;
}; // class suffix_array

/************************************************
 * Implementation: class suffix_array
 ************************************************/

inline suffix_array::suffix_array(void)
    : sentinel_pos_(0), sentinel_rank_(0)
{
    // do nothing
}

template <typename InputIterator>
void suffix_array::insert(InputIterator begin, InputIterator end)
{
    if (begin == end) { return; }

    auto seq_it = begin;
    decltype(lf(0)) kp, psi_kp;
    if (size() > 0)
    {
        psi_kp = sentinel_pos_;
        kp = wt_.lf(sentinel_pos_) + 1;
        if (kp <= psi_kp) { ++psi_kp; }
    }
    else
    {
        // insert first sampled point at last position
        assert(*seq_it != 0);
        insert_term(0, *seq_it, true);
        pi_.insert(0, 0);
        lcpa_.insert(lcpa_.begin(), 0);
        kp = 1;
        psi_kp = 0;
        ++seq_it;
    }

    typename decltype(lcpa_)::value_type lcp = 0;
    while (seq_it != end)
    {
        assert(*seq_it != 0);
        insert_term(kp, *seq_it, false);

        auto lf_kp = wt_.lf(kp) + 1;
        insert_lcp(kp, psi_kp, lf_kp, lcp);
        psi_kp = kp;
        kp = lf_kp;
        if (kp <= psi_kp) { ++psi_kp; }

        ++seq_it;
    }

    insert_term(kp, 0, false);
    insert_lcp(kp, psi_kp, 0, lcp);

    sentinel_pos_ = kp;
    sentinel_rank_ = wt_.rank(kp, 0);

    add_samples(0);
}

inline suffix_array::size_type suffix_array::size(void) const
{
    return wt_.size();
}

inline suffix_array::size_type suffix_array::lcp(size_type i) const
{
    return lcpa_[i];
}

inline suffix_array::term_type suffix_array::bwt(size_type i) const
{
    return wt_[i];
}

inline suffix_array::term_type suffix_array::term(value_type j) const
{
    return wt_.search(rank(j) + 1);
}

inline suffix_array::size_type suffix_array::psi(size_type i) const
{
    if (i == 0) { return sentinel_pos_; }

    return i < sentinel_rank_
        ? wt_.select(i - 1, 0)
        : wt_.psi(i);
}

inline suffix_array::size_type suffix_array::lf(size_type i) const
{
    if (i == sentinel_pos_) { return 0; }

    auto pair = wt_.access_and_lf(i);
    return (pair.first == 0 && i < sentinel_pos_) + pair.second;
}

inline suffix_array::value_type suffix_array::operator[](size_type i) const
{
    return at(i);
}

inline void suffix_array::insert_term(size_type i, term_type c, bool is_sampled)
{
    wt_.insert(i, c);
    sa_samples_.insert(i, is_sampled);
    isa_samples_.insert(0, is_sampled);
}

} // namespace impl

} // namespace desa

#endif // DESA_SUFFIX_ARRAY_HPP_
