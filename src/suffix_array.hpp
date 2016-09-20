/************************************************
 *  suffix_array.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_SUFFIX_ARRAY_HPP_
#define DESA_SUFFIX_ARRAY_HPP_

#include <vector>

#include "bit_vector.hpp"
#include "permutation.hpp"
#include "wavelet_tree.hpp"

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

    template <typename Sequence>
    void insert(value_type j, Sequence const &s);
    void erase(value_type j, size_type m);

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
    void insert_term(size_type i, value_type j, term_type c);
    term_type erase_term(size_type i, value_type j);
    void reorder(size_type actual, size_type expected);
    void add_samples(value_type j);

private: // Private Static Property(ies)
    static constexpr size_type MAX_SAMPLE_DISTANCE = 100;
    static constexpr size_type BIT_BLOCK_SIZE = 64;

private: // Private Property(ies)
    wavelet_tree<term_type> wt_;
    bit_vector<BIT_BLOCK_SIZE> isa_samples_;
    bit_vector<BIT_BLOCK_SIZE> sa_samples_;
    permutation pi_;
    ::std::vector<size_type> lcpa_;
}; // class suffix_array

/************************************************
 * Implementation: class suffix_array
 ************************************************/

inline suffix_array::suffix_array(void)
{
    wt_.insert(0, 0);
    sa_samples_.insert(0, true);
    isa_samples_.insert(0, true);
    pi_.insert(0, 0);
}

template <typename Sequence>
void suffix_array::insert(value_type j, Sequence const &s)
{
    auto it = s.crbegin();
    if (it == s.crend()) { return; }

    auto i = rank(j);
    auto k = lf(i);
    auto c = wt_.erase(i);
    wt_.insert(i, *it);

    auto kp = lf(i);
    if (c < *it) { ++kp; }
    while (++it != s.crend())
    {
        if (k >= kp) { ++k; }

        insert_term(kp, j, *it);
        kp = lf(kp);

        if (c < *it) { ++kp; }
    }

    if (k >= kp) { ++k; }
    insert_term(kp, j, c);

    reorder(k, lf(kp));
    add_samples(j);
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
    auto i = rank(j + 1 < size() ? j + 1 : 0);
    return wt_[i];
}

inline suffix_array::size_type suffix_array::psi(size_type i) const
{
    return wt_.psi(i);
}

inline suffix_array::size_type suffix_array::lf(size_type i) const
{
    return wt_.lf(i);
}

inline suffix_array::value_type suffix_array::operator[](size_type i) const
{
    return at(i);
}

inline void suffix_array::insert_term(size_type i, value_type j, term_type c)
{
    wt_.insert(i, c);
    sa_samples_.insert(i, false);
    isa_samples_.insert(j, false);
}

inline suffix_array::term_type suffix_array::erase_term(size_type i, value_type j)
{
    sa_samples_.erase(i);
    auto b = isa_samples_.erase(j);
    if (b)
    {
        auto r = i > 0 ? sa_samples_.rank(i - 1, true) : 0;
        pi_.erase(r);
    }

    return wt_.erase(i);
}

} // namespace impl

} // namespace desa

#endif // DESA_SUFFIX_ARRAY_HPP_
