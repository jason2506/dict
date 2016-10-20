/************************************************
 *  with_csa.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_WITH_CSA_HPP_
#define DESA_WITH_CSA_HPP_

#include "internal/bit_vector.hpp"
#include "internal/permutation.hpp"

namespace desa
{

/************************************************
 * Declaration: class with_csa<TI, T>
 ************************************************/

template <typename TextIndex, typename Trait>
class with_csa
{
public: // Public Type(s)
    typedef typename Trait::size_type size_type;
    typedef typename Trait::size_type value_type;
    typedef typename Trait::term_type term_type;

private: // Private Types(s)
    typedef typename Trait::wt_type wt_type;
    typedef typename Trait::event event;

public: // Public Method(s)
    with_csa(wt_type const &wt);

    value_type at(size_type i) const;
    size_type rank(value_type j) const;
    term_type term(value_type j) const;

    value_type operator[](size_type i) const;

protected: // Protected Method(s)
    void update(typename event::after_inserting_first_term);
    void update(typename event::after_inserting_term info);
    void update(typename event::after_inserting_sequence);

private: // Private Method(s)
    void insert_term(size_type i, bool is_sampled);
    void add_samples(value_type j);

private: // Private Static Property(ies)
    static constexpr size_type MAX_SAMPLE_DISTANCE = 100;
    static constexpr size_type BIT_BLOCK_SIZE = 64;

private: // Private Property(ies)
    wt_type const &wt_;
    internal::bit_vector<BIT_BLOCK_SIZE> isa_samples_;
    internal::bit_vector<BIT_BLOCK_SIZE> sa_samples_;
    internal::permutation pi_;
}; // class with_csa<TI, T>

/************************************************
 * Implementation: class with_csa<TI, T>
 ************************************************/

template <typename TI, typename T>
inline with_csa<TI, T>::with_csa(wt_type const &wt)
    : wt_(wt)
{
    // do nothing
}

template <typename TI, typename T>
typename with_csa<TI, T>::value_type with_csa<TI, T>::at(size_type i) const
{
    size_type off = 0;
    while (!sa_samples_[i])
    {
        i = static_cast<TI const *>(this)->lf(i);
        ++off;
    }

    auto r = sa_samples_.rank(i, true);
    auto j = pi_.at(r - 1);

    auto sa = isa_samples_.select(j, true) + off;
    auto n = static_cast<TI const *>(this)->size();
    return sa < n ? sa : sa - n;
}

template <typename TI, typename T>
typename with_csa<TI, T>::size_type with_csa<TI, T>::rank(value_type j) const
{
    auto br_pair = isa_samples_.access_and_rank(j, true);
    auto b = br_pair.first;
    auto r = br_pair.second;
    if (b)
    {
        // retrieve a value at a sampled position
        auto i = pi_.rank(r - 1);
        return sa_samples_.select(i, true);
    }
    else
    {
        // retrieve a value at a unsampled position
        auto off = isa_samples_.select(r, true) - j;
        auto i = pi_.rank(r);
        auto v = sa_samples_.select(i, true);
        for (decltype(off) t = 0; t < off; ++t)
        {
            v = static_cast<TI const *>(this)->lf(v);
        }

        return v;
    }
}

template <typename TI, typename T>
inline typename with_csa<TI, T>::term_type with_csa<TI, T>::term(value_type j) const
{
    return wt_.search(rank(j) + 1);
}

template <typename TI, typename T>
inline typename with_csa<TI, T>::value_type with_csa<TI, T>::operator[](size_type i) const
{
    return at(i);
}

template <typename TI, typename T>
inline void with_csa<TI, T>::update(typename event::after_inserting_first_term)
{
    insert_term(0, true);
    pi_.insert(0, 0);
}

template <typename TI, typename T>
inline void with_csa<TI, T>::update(typename event::after_inserting_term info)
{
    insert_term(info.kp, false);
}

template <typename TI, typename T>
inline void with_csa<TI, T>::update(typename event::after_inserting_sequence)
{
    add_samples(0);
}

template <typename TI, typename T>
inline void with_csa<TI, T>::insert_term(size_type i, bool is_sampled)
{
    sa_samples_.insert(i, is_sampled);
    isa_samples_.insert(0, is_sampled);
}

template <typename TI, typename T>
void with_csa<TI, T>::add_samples(value_type j)
{
    auto n = static_cast<TI const *>(this)->size();
    if (j + 1 == n) { return; }

    auto r = isa_samples_.rank(j, true);
    auto left_sample_pos = r > 0 ? isa_samples_.select(r - 1, true) + 1 : 0;
    auto right_sample_pos = isa_samples_.select(r, true);

    auto p = right_sample_pos;
    while (p > left_sample_pos + MAX_SAMPLE_DISTANCE)
    {
        p -= MAX_SAMPLE_DISTANCE + 1;

        auto i = rank(p);
        auto k = i > 0 ? sa_samples_.rank(i - 1, true) : 0;
        sa_samples_.set(i);
        isa_samples_.set(p);
        pi_.insert(k, r);
    }
}

} // namespace desa

#endif // DESA_WITH_CSA_HPP_
