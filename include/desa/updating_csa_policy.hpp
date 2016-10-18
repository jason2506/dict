/************************************************
 *  updating_csa_policy.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_UPDATING_CSA_POLICY_HPP_
#define DESA_UPDATING_CSA_POLICY_HPP_

#include "bit_vector.hpp"
#include "permutation.hpp"
#include "wavelet_tree.hpp"

namespace desa
{

namespace impl
{

/************************************************
 * Declaration: class updating_csa_policy<TI>
 ************************************************/

template <typename TextIndex>
class updating_csa_policy
{
public: // Public Type(s)
    typedef ::std::size_t size_type;
    typedef ::std::size_t value_type;
    typedef ::std::uint16_t term_type;

public: // Public Method(s)
    updating_csa_policy(wavelet_tree<term_type> const &wt);

    value_type at(size_type i) const;
    size_type rank(value_type j) const;
    term_type term(value_type j) const;

    value_type operator[](size_type i) const;

protected: // Protected Method(s)
    void update_after_inserting_first_term(void);
    void update_after_inserting_term(size_type kp, size_type psi_kp, size_type lf_kp);
    void update_after_inserting_sequence(void);

private: // Private Method(s)
    void insert_term(size_type i, bool is_sampled);
    void add_samples(value_type j);

private: // Private Static Property(ies)
    static constexpr size_type MAX_SAMPLE_DISTANCE = 100;
    static constexpr size_type BIT_BLOCK_SIZE = 64;

private: // Private Property(ies)
    wavelet_tree<term_type> const &wt_;
    bit_vector<BIT_BLOCK_SIZE> isa_samples_;
    bit_vector<BIT_BLOCK_SIZE> sa_samples_;
    permutation pi_;
}; // class updating_csa_policy<TI>

/************************************************
 * Implementation: class updating_csa_policy<TI>
 ************************************************/

template <typename TI>
inline updating_csa_policy<TI>::updating_csa_policy(wavelet_tree<term_type> const &wt)
    : wt_(wt)
{
    // do nothing
}

template <typename TI>
typename updating_csa_policy<TI>::value_type updating_csa_policy<TI>::at(size_type i) const
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

template <typename TI>
typename updating_csa_policy<TI>::size_type updating_csa_policy<TI>::rank(value_type j) const
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

template <typename TI>
inline typename updating_csa_policy<TI>::term_type updating_csa_policy<TI>::term(value_type j) const
{
    return wt_.search(rank(j) + 1);
}

template <typename TI>
inline typename updating_csa_policy<TI>::value_type updating_csa_policy<TI>::operator[](size_type i) const
{
    return at(i);
}

template <typename TI>
inline void updating_csa_policy<TI>::update_after_inserting_first_term(void)
{
    insert_term(0, true);
    pi_.insert(0, 0);
}

template <typename TI>
inline void updating_csa_policy<TI>::update_after_inserting_term(size_type kp, size_type psi_kp, size_type lf_kp)
{
    insert_term(kp, false);
}

template <typename TI>
inline void updating_csa_policy<TI>::update_after_inserting_sequence(void)
{
    add_samples(0);
}

template <typename TI>
inline void updating_csa_policy<TI>::insert_term(size_type i, bool is_sampled)
{
    sa_samples_.insert(i, is_sampled);
    isa_samples_.insert(0, is_sampled);
}

template <typename TI>
void updating_csa_policy<TI>::add_samples(value_type j)
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

} // namespace impl

} // namespace desa

#endif // DESA_UPDATING_CSA_POLICY_HPP_
