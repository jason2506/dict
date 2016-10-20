/************************************************
 *  with_lcp.hpp
 *  DESA
 *
 *  Copyright (c) 2015-2016, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_WITH_LCP_HPP_
#define DESA_WITH_LCP_HPP_

#include "internal/lcp_trait.hpp"
#include "internal/tree_list.hpp"

namespace desa
{

/************************************************
 * Declaration: class with_lcp<UPs...>
 ************************************************/

template <template <typename, typename> class... UpdatingPolicies>
struct with_lcp
{
    template <typename TextIndex, typename Trait>
    class policy;
}; // class with_lcp<UPs...>

/************************************************
 * Declaration: class with_lcp<UPs...>::policy<TI, T>
 ************************************************/

template <template <typename, typename> class... UPs>
template <typename TextIndex, typename Trait>
class with_lcp<UPs...>::policy : public internal::chained_updater<UPs...>::template updater
    <
        policy<TextIndex, Trait>,
        internal::lcp_trait<Trait>
    >
{
public: // Public Type(s)
    using size_type = typename Trait::size_type;

private: // Private Types(s)
    using wt_type = typename Trait::wt_type;
    using event = typename Trait::event;

    using lcp_trait = internal::lcp_trait<Trait>;
    using updating_policies = typename internal::chained_updater<UPs...>::template updater<policy, lcp_trait>;

public: // Public Method(s)
    policy(wt_type const &wt);

    size_type lcp(size_type i) const;

protected: // Protected Method(s)
    void update(typename event::after_inserting_first_term);
    void update(typename event::after_inserting_term info);
    void update(typename event::after_inserting_sequence);

private: // Private Property(ies)
    wt_type const &wt_;
    internal::tree_list lcpa_;
}; // class with_lcp<UPs...>::policy<TI, T>

/************************************************
 * Implementation: class with_lcp<UPs...>::policy<TI, T>
 ************************************************/

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
inline with_lcp<UPs...>::policy<TI, T>::policy(wt_type const &wt)
    : wt_(wt)
{
    // do nothing
}

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
inline typename with_lcp<UPs...>::template policy<TI, T>::size_type
with_lcp<UPs...>::policy<TI, T>::lcp(size_type i) const
{
    return lcpa_[i];
}

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
inline void with_lcp<UPs...>::policy<TI, T>::update(typename event::after_inserting_first_term)
{
    lcpa_.insert(lcpa_.begin(), 0);
}

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
void with_lcp<UPs...>::policy<TI, T>::update(typename event::after_inserting_term info)
{
    static typename decltype(lcpa_)::size_type lcp = 0;
    auto pos = info.pos, psi_pos = info.psi_pos, lf_pos = info.lf_pos;

    auto psi = [&](size_type x) {
        return x == 0 ? pos : wt_.psi(x - (x < lf_pos));
    };

    auto term_at_f = [&](size_type x) {
        return x == 0 ? 0 : wt_.search(x + 1 - (x < lf_pos));
    };

    // calculate LCP[pos]
    auto lcpa_it = lcpa_.find(pos);
    auto old_lcp = lcpa_it ? *lcpa_it : 0;
    if (pos > 0 && psi_pos > 0 && psi(pos - 1) == psi_pos - 1)
    {
        auto c = term_at_f(pos);
        if (c != 0 && c == term_at_f(pos - 1))   { ++lcp; }
        else                                    { lcp = 0; }
    }
    else
    {
        auto x = pos, y = pos - 1;
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
        // re-calculate LCP[pos + 1]
        auto &lcp = *lcpa_it;
        auto x = pos + 1, y = pos;
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
    updating_policies::update(
        typename lcp_trait::event::after_inserting_lcp{
            pos, lcp, lcpa_it ? *lcpa_it : 0
        }
    );
}

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
inline void with_lcp<UPs...>::policy<TI, T>::update(typename event::after_inserting_sequence)
{
    // do nothing
}

} // namespace desa

#endif // DESA_WITH_LCP_HPP_
