/************************************************
 *  with_lcp.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_WITH_LCP_HPP_
#define DICT_WITH_LCP_HPP_

#include <iterator>

#include "internal/lcp_trait.hpp"
#include "internal/tree_list.hpp"

namespace dict {

/************************************************
 * Declaration: class with_lcp<UPs...>
 ************************************************/

template <template <typename, typename> class... UpdatingPolicies>
struct with_lcp {
    template <typename TextIndex, typename Trait>
    class policy;
};  // class with_lcp<UPs...>

/************************************************
 * Declaration: class with_lcp<UPs...>::policy<TI, T>
 ************************************************/

template <template <typename, typename> class... UPs>
template <typename TextIndex, typename Trait>
class with_lcp<UPs...>::policy : public internal::chained_updater<UPs...>
    ::template updater<
        policy<TextIndex, Trait>,
        internal::lcp_trait<Trait>
    > {
 public:  // Public Type(s)
    using host_type = TextIndex;
    using size_type = typename Trait::size_type;

 private:  // Private Types(s)
    using wm_type = typename Trait::wm_type;
    using event = typename Trait::event;

    using lcp_trait = internal::lcp_trait<Trait>;
    using updating_policies = typename internal::chained_updater<UPs...>
        ::template updater<policy, lcp_trait>;

 public:  // Public Method(s)
    policy(wm_type const &wt);

    size_type lcp(size_type i) const;

 protected:  // Protected Method(s)
    template <typename Sequence>
    void update(typename event::template after_inserting_first_term<Sequence> const &info);
    template <typename Sequence>
    void update(typename event::template after_inserting_term<Sequence> const &info);
    template <typename Sequence>
    void update(typename event::template after_inserting_sequence<Sequence> const &);

 private:  // Private Property(ies)
    wm_type const &wm_;
    internal::tree_list lcpa_;
    size_type psi_lcp_;
};  // class with_lcp<UPs...>::policy<TI, T>

/************************************************
 * Implementation: class with_lcp<UPs...>::policy<TI, T>
 ************************************************/

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
inline with_lcp<UPs...>::policy<TI, T>::policy(wm_type const &wt)
    : wm_(wt), lcpa_(), psi_lcp_(0) {
    // do nothing
}

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
inline typename with_lcp<UPs...>::template policy<TI, T>::size_type
with_lcp<UPs...>::policy<TI, T>::lcp(size_type i) const {
    return lcpa_[i];
}

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
template <typename Sequence>
inline void with_lcp<UPs...>::policy<TI, T>::update(
        typename event::template after_inserting_first_term<Sequence> const &info) {
    assert(psi_lcp_ == 0);
    lcpa_.insert(lcpa_.begin(), 0);
    updating_policies::update(typename lcp_trait::event::template after_inserting_lcp<Sequence>{
        info.s, 0,
        0, 0, 0
    });
}

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
template <typename Sequence>
void with_lcp<UPs...>::policy<TI, T>::update(
        typename event::template after_inserting_term<Sequence> const &info) {
    auto pos = info.pos, psi_pos = info.psi_pos, lf_pos = info.lf_pos;
    auto num_inserted = info.num_inserted;
    auto s_rend = std::rbegin(info.s) + num_inserted - 1;
    auto const &wm = wm_;

    auto psi = [pos, lf_pos, &wm](size_type x) {
        return x == 0 ? pos : wm.psi(x - (x < lf_pos));
    };

    auto psi_hint = [pos, lf_pos, &wm](size_type x, typename wm_type::value_type hint) {
        return x == 0 ? pos : wm.psi(x - (x < lf_pos), hint);
    };

    auto term_at_f = [lf_pos, &wm](size_type x) {
        return x == 0 ? 0 : wm.search(x + 1 - (x < lf_pos));
    };

    // calculate LCP[pos]
    auto lcpa_it = lcpa_.find(pos);
    auto old_lcp = lcpa_it ? *lcpa_it : 0;
    assert(pos > 0);
    if (psi_pos > 0 && psi(pos - 1) == psi_pos - 1) {
        if (num_inserted > 0 && *s_rend == term_at_f(pos - 1)) {
            ++psi_lcp_;
        } else {
            psi_lcp_ = 0;
        }
    } else {
        auto s_it = s_rend;
        auto x = pos - 1;
        for (psi_lcp_ = 0; psi_lcp_ < old_lcp; ++psi_lcp_) {
            x = psi_hint(x, *s_it);
            --s_it;
        }

        while (psi_lcp_ < num_inserted && *s_it == term_at_f(x)) {
            x = psi_hint(x, *s_it);
            ++psi_lcp_;
            --s_it;
        }
    }

    if (lcpa_it && old_lcp == psi_lcp_) {
        // re-calculate LCP[pos + 1]
        auto &next_lcp = *lcpa_it;
        auto s_it = s_rend;
        auto x = pos + 1;
        for (next_lcp = 0; next_lcp < old_lcp; ++next_lcp) {
            x = psi_hint(x, *s_it);
            --s_it;
        }

        while (next_lcp < num_inserted && *s_it == term_at_f(x)) {
            x = psi_hint(x, *s_it);
            ++next_lcp;
            --s_it;
        }
    }

    lcpa_.insert(lcpa_it, psi_lcp_);
    updating_policies::update(
        typename lcp_trait::event::template after_inserting_lcp<Sequence>{
            info.s, info.num_inserted,
            pos, psi_lcp_, lcpa_it ? *lcpa_it : 0
        });
}

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
template <typename Sequence>
inline void with_lcp<UPs...>::policy<TI, T>::update(
        typename event::template after_inserting_sequence<Sequence> const &) {
    // do nothing
}

}  // namespace dict

#endif  // DICT_WITH_LCP_HPP_
