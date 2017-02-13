/************************************************
 *  with_lcp.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_WITH_LCP_HPP_
#define DICT_WITH_LCP_HPP_

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
    size_type lcp_;
};  // class with_lcp<UPs...>::policy<TI, T>

/************************************************
 * Implementation: class with_lcp<UPs...>::policy<TI, T>
 ************************************************/

template <template <typename, typename> class... UPs>
template <typename TI, typename T>
inline with_lcp<UPs...>::policy<TI, T>::policy(wm_type const &wt)
    : wm_(wt), lcp_(0) {
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
    // lcp_ = 0;
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
    if (pos > 0 && psi_pos > 0 && psi(pos - 1) == psi_pos - 1) {
        auto c = term_at_f(pos);
        if (c != 0 && c == term_at_f(pos - 1)) {
            ++lcp_;
        } else {
            lcp_ = 0;
        }
    } else {
        auto x = pos, y = pos - 1;
        for (lcp_ = 0; lcp_ < old_lcp; ++lcp_) {
            x = psi(x);
            y = psi(y);
        }

        auto cx = term_at_f(x);
        decltype(cx) cy;
        while (cx != 0 && cx == (cy = term_at_f(y))) {
            x = psi_hint(x, cx);
            y = psi_hint(y, cy);
            cx = term_at_f(x);
            ++lcp_;
        }
    }

    if (lcpa_it && old_lcp == lcp_) {
        // re-calculate LCP[pos + 1]
        auto &lcp = *lcpa_it;
        auto x = pos + 1, y = pos;
        for (lcp = 0; lcp < old_lcp; ++lcp) {
            x = psi(x);
            y = psi(y);
        }

        auto cx = term_at_f(x);
        decltype(cx) cy;
        while (cx != 0 && cx == (cy = term_at_f(y))) {
            x = psi_hint(x, cx);
            y = psi_hint(y, cy);
            cx = term_at_f(x);
            ++lcp;
        }
    }

    lcpa_.insert(lcpa_it, lcp_);
    updating_policies::update(
        typename lcp_trait::event::template after_inserting_lcp<Sequence>{
            info.s, info.num_inserted,
            pos, lcp_, lcpa_it ? *lcpa_it : 0
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
