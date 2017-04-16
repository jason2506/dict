/************************************************
 *  with_lcp_impl.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_INTERNAL_WITH_LCP_IMPL_HPP_
#define DICT_INTERNAL_WITH_LCP_IMPL_HPP_

#include <iterator>

#include "chained_updater.hpp"
#include "lcp_trait.hpp"
#include "tree_list.hpp"
#include "type_list.hpp"

namespace dict {

namespace internal {

/************************************************
 * Declaration: class with_lcp_impl<TI, T, UPs...>
 ************************************************/

template <
    typename TextIndex,
    typename Trait,
    template <typename, typename> class... UpdatingPolicies
>
class with_lcp_impl : public chained_updater<
    type_list<
        with_lcp_impl<TextIndex, Trait>,
        lcp_trait<Trait>
    >,
    UpdatingPolicies...
> {
 public:  // Public Type(s)
    using host_type = TextIndex;
    using size_type = typename Trait::size_type;

 private:  // Private Types(s)
    using helper = typename Trait::helper;
    using event = typename Trait::event;

    using lcp_trait = lcp_trait<Trait>;
    using updating_policies = chained_updater<
            type_list<
                with_lcp_impl<TextIndex, Trait>,
                lcp_trait
            >,
            UpdatingPolicies...
        >;

 public:  // Public Method(s)
    size_type lcp(size_type i) const;

 protected:  // Protected Method(s)
    template <typename Sequence>
    void update(typename event::template after_inserting_first_term<Sequence> const &info);
    template <typename Sequence>
    void update(typename event::template after_inserting_term<Sequence> const &info);
    template <typename Sequence>
    void update(typename event::template after_inserting_sequence<Sequence> const &);

    void update(typename event::before_erasuring_sequence const &);
    void update(typename event::after_erasuring_term const &info);
    void update(typename event::after_moving_term const &);
    void update(typename event::after_erasuring_sequence const &);

 private:  // Private Property(ies)
    tree_list lcpa_;
    size_type psi_lcp_;
};  // class with_lcp_impl<TI, T, UPs...>

/************************************************
 * Implementation: class with_lcp_impl<TI, T, UPs...>
 ************************************************/

template <typename TI, typename T, template <typename, typename> class... UPs>
inline typename with_lcp_impl<TI, T, UPs...>::size_type
with_lcp_impl<TI, T, UPs...>::lcp(size_type i) const {
    return lcpa_[i];
}

template <typename TI, typename T, template <typename, typename> class... UPs>
template <typename Sequence>
inline void with_lcp_impl<TI, T, UPs...>::update(
        typename event::template after_inserting_first_term<Sequence> const &info) {
    assert(psi_lcp_ == 0);
    lcpa_.insert(lcpa_.begin(), 0);
    updating_policies::update(typename lcp_trait::event::template after_inserting_lcp<Sequence>{
        info.s, 0,
        0, 0, 0
    });
}

template <typename TI, typename T, template <typename, typename> class... UPs>
template <typename Sequence>
void with_lcp_impl<TI, T, UPs...>::update(
        typename event::template after_inserting_term<Sequence> const &info) {
    auto pos = info.pos, psi_pos = info.psi_pos, lf_pos = info.lf_pos;
    auto num_inserted = info.num_inserted;
    auto s_rend = std::rbegin(info.s) + num_inserted - 1;
    auto const &wm = helper::get_wm(this);

    auto psi = [pos, lf_pos, &wm](size_type x) {
        return x == 0 ? pos : wm.psi(x - (x < lf_pos));
    };

    auto psi_hint = [pos, lf_pos, &wm](size_type x, decltype(wm[0]) hint) {
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

template <typename TI, typename T, template <typename, typename> class... UPs>
template <typename Sequence>
inline void with_lcp_impl<TI, T, UPs...>::update(
        typename event::template after_inserting_sequence<Sequence> const &) {
    // do nothing
}

template <typename TI, typename T, template <typename, typename> class... UPs>
inline void with_lcp_impl<TI, T, UPs...>::update(
        typename event::before_erasuring_sequence const &) {
    // do nothing
}

template <typename TI, typename T, template <typename, typename> class... UPs>
inline void with_lcp_impl<TI, T, UPs...>::update(
        typename event::after_erasuring_term const &info) {
    auto it = lcpa_.find(info.pos);
    auto lcp = *it;
    auto next_it = lcpa_.erase(it);
    auto next_lcp = next_it ? *next_it : 0;
    if (next_lcp > lcp) { *next_it = lcp; }

    updating_policies::update(
        typename lcp_trait::event::after_erasing_lcp{
            info.s, info.pos, lcp, next_lcp
        });
}

template <typename TI, typename T, template <typename, typename> class... UPs>
inline void with_lcp_impl<TI, T, UPs...>::update(
        typename event::after_moving_term const &) {
    // do nothing
}

template <typename TI, typename T, template <typename, typename> class... UPs>
inline void with_lcp_impl<TI, T, UPs...>::update(
        typename event::after_erasuring_sequence const &) {
    // do nothing
}

}  // namespace internal

}  // namespace dict

#endif  // DICT_INTERNAL_WITH_LCP_IMPL_HPP_
