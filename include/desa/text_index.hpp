/************************************************
 *  text_index.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_TEXT_INDEX_HPP_
#define DESA_TEXT_INDEX_HPP_

#include <cassert>

#include "internal/wavelet_tree.hpp"

#include "updating_nothing_policy.hpp"

namespace desa
{

/************************************************
 * Declaration: class text_index<UP>
 ************************************************/

template <template <typename> class UpdatingPolicy = updating_nothing_policy>
class text_index : public UpdatingPolicy<text_index<UpdatingPolicy>>
{
public: // Public Type(s)
    typedef ::std::size_t size_type;
    typedef ::std::uint16_t term_type;

public: // Public Method(s)
    text_index(void);

    template <typename InputIterator>
    void insert(InputIterator begin, InputIterator end);

    size_type size(void) const;
    term_type bwt(size_type i) const;
    size_type psi(size_type i) const;
    size_type lf(size_type i) const;

protected: // Protected Method(s)
    // void update_after_inserting_first_term(void);
    // void update_after_inserting_term(size_type kp, size_type psi_kp, size_type lf_kp);
    // void update_after_inserting_sequence(void);

private: // Private Type(s)
    typedef UpdatingPolicy<text_index> updating_policy;

private: // Private Property(ies)
    internal::wavelet_tree<term_type> wt_;
    size_type sentinel_pos_;
    size_type sentinel_rank_;
}; // class text_index<UP>

/************************************************
 * Implementation: class text_index<UP>
 ************************************************/

template <template <typename> class UP>
inline text_index<UP>::text_index(void)
    : updating_policy(wt_), sentinel_pos_(0), sentinel_rank_(0)
{
    // do nothing
}

template <template <typename> class UP>
template <typename InputIterator>
void text_index<UP>::insert(InputIterator begin, InputIterator end)
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
        wt_.insert(0, *seq_it);

        updating_policy::update_after_inserting_first_term();

        kp = 1;
        psi_kp = 0;
        ++seq_it;
    }

    while (seq_it != end)
    {
        assert(*seq_it != 0);
        wt_.insert(kp, *seq_it);

        auto lf_kp = wt_.lf(kp) + 1;
        updating_policy::update_after_inserting_term(kp, psi_kp, lf_kp);

        psi_kp = kp;
        kp = lf_kp;
        if (kp <= psi_kp) { ++psi_kp; }

        ++seq_it;
    }

    wt_.insert(kp, 0);

    updating_policy::update_after_inserting_term(kp, psi_kp, 0);

    sentinel_pos_ = kp;
    sentinel_rank_ = wt_.rank(kp, 0);

    updating_policy::update_after_inserting_sequence();
}

template <template <typename> class UP>
inline typename text_index<UP>::size_type text_index<UP>::size(void) const
{
    return wt_.size();
}

template <template <typename> class UP>
inline typename text_index<UP>::term_type text_index<UP>::bwt(size_type i) const
{
    return wt_[i];
}

template <template <typename> class UP>
inline typename text_index<UP>::size_type text_index<UP>::psi(size_type i) const
{
    if (i == 0) { return sentinel_pos_; }

    return i < sentinel_rank_
        ? wt_.select(i - 1, 0)
        : wt_.psi(i);
}

template <template <typename> class UP>
inline typename text_index<UP>::size_type text_index<UP>::lf(size_type i) const
{
    if (i == sentinel_pos_) { return 0; }

    auto pair = wt_.access_and_lf(i);
    return (pair.first == 0 && i < sentinel_pos_) + pair.second;
}

} // namespace desa

#endif // DESA_TEXT_INDEX_HPP_
