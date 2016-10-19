/************************************************
 *  updating_lcp_policy.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_UPDATING_LCP_POLICY_HPP_
#define DESA_UPDATING_LCP_POLICY_HPP_

#include "internal/tree_list.hpp"
#include "internal/wavelet_tree.hpp"

namespace desa
{

/************************************************
 * Declaration: class updating_lcp_policy<TI>
 ************************************************/

template <typename TextIndex>
class updating_lcp_policy
{
public: // Public Type(s)
    typedef ::std::size_t size_type;
    typedef ::std::uint16_t term_type;

public: // Public Method(s)
    updating_lcp_policy(internal::wavelet_tree<term_type> const &wt);

    size_type lcp(size_type i) const;

protected: // Protected Method(s)
    void update_after_inserting_first_term(void);
    void update_after_inserting_term(size_type kp, size_type psi_kp, size_type lf_kp);
    void update_after_inserting_sequence(void);

private: // Private Property(ies)
    internal::wavelet_tree<term_type> const &wt_;
    internal::tree_list lcpa_;
}; // class updating_lcp_policy<TI>

/************************************************
 * Implementation: class updating_lcp_policy<TI>
 ************************************************/

template <typename TI>
inline updating_lcp_policy<TI>::updating_lcp_policy(internal::wavelet_tree<term_type> const &wt)
    : wt_(wt)
{
    // do nothing
}

template <typename TI>
inline typename updating_lcp_policy<TI>::size_type updating_lcp_policy<TI>::lcp(size_type i) const
{
    return lcpa_[i];
}

template <typename TI>
inline void updating_lcp_policy<TI>::update_after_inserting_first_term(void)
{
    lcpa_.insert(lcpa_.begin(), 0);
}

template <typename TI>
void updating_lcp_policy<TI>::update_after_inserting_term(size_type kp, size_type psi_kp, size_type lf_kp)
{
    static typename decltype(lcpa_)::size_type lcp = 0;

    auto psi = [&](size_type x) {
        return x == 0 ? kp : wt_.psi(x - (x < lf_kp));
    };

    auto term_at_f = [&](size_type x) {
        return x == 0 ? 0 : wt_.search(x + 1 - (x < lf_kp));
    };

    // calculate LCP[kp]
    auto lcpa_it = lcpa_.find(kp);
    auto old_lcp = lcpa_it ? *lcpa_it : 0;
    if (kp > 0 && psi_kp > 0 && psi(kp - 1) == psi_kp - 1)
    {
        auto c = term_at_f(kp);
        if (c != 0 && c == term_at_f(kp - 1))   { ++lcp; }
        else                                    { lcp = 0; }
    }
    else
    {
        auto x = kp, y = kp - 1;
        for (lcp = 0; lcp < old_lcp; ++lcp)
        {
            x = psi(x);
            y = psi(y);
        }

        auto c = term_at_f(x);
        while (c != 0 && c == term_at_f(y))
        {
            x = psi(x);
            y = psi(y);
            c = term_at_f(x);
            ++lcp;
        }
    }

    if (lcpa_it && old_lcp == lcp)
    {
        // re-calculate LCP[kp + 1]
        auto &lcp = *lcpa_it;
        auto x = kp + 1, y = kp;
        for (lcp = 0; lcp < old_lcp; ++lcp)
        {
            x = psi(x);
            y = psi(y);
        }

        auto c = term_at_f(x);
        while (c != 0 && c == term_at_f(y))
        {
            x = psi(x);
            y = psi(y);
            c = term_at_f(x);
            ++lcp;
        }
    }

    lcpa_.insert(lcpa_it, lcp);
    // return lcpa_it ? *lcpa_it : 0;
}

template <typename TI>
inline void updating_lcp_policy<TI>::update_after_inserting_sequence(void)
{
    // do nothing
}

} // namespace desa

#endif // DESA_UPDATING_LCP_POLICY_HPP_
