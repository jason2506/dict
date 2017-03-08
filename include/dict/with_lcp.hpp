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

using internal::lcp_trait;

/************************************************
 * Declaration: class with_lcp<TI, AF>
 ************************************************/

template <typename TextIndex, typename AccessorFactory>
class with_lcp {
 public:  // Public Type(s)
    using host_type = TextIndex;
    using size_type = text_index_trait::size_type;

 private:  // Private Types(s)
    using accessor_factory = AccessorFactory;
    using event = text_index_trait::event;

 public:  // Public Method(s)
    size_type lcp(size_type i) const;

 protected:  // Protected Method(s)
    template <typename Sequence>
    void update(typename event::template after_inserting_first_term<Sequence> const &info);
    template <typename Sequence>
    void update(typename event::template after_inserting_term<Sequence> const &info);
    template <typename Event>
    void update(Event const &);

 private:  // Private Property(ies)
    internal::tree_list lcpa_;
    size_type psi_lcp_;
};  // class with_lcp<TI, AF>

/************************************************
 * Implementation: class with_lcp<TI, AF>
 ************************************************/

template <typename TI, typename AF>
inline typename with_lcp<TI, AF>::size_type
with_lcp<TI, AF>::lcp(size_type i) const {
    return lcpa_[i];
}

template <typename TI, typename AF>
template <typename Sequence>
inline void with_lcp<TI, AF>::update(
        typename event::template after_inserting_first_term<Sequence> const &info) {
    assert(psi_lcp_ == 0);
    lcpa_.insert(lcpa_.begin(), 0);
    accessor_factory::template create<host_type>().notify(this,
        typename lcp_trait::event::template after_inserting_lcp<Sequence>{
            info.s, 0,
            0, 0, 0
        });
}

template <typename TI, typename AF>
template <typename Sequence>
void with_lcp<TI, AF>::update(
        typename event::template after_inserting_term<Sequence> const &info) {
    auto pos = info.pos, psi_pos = info.psi_pos, lf_pos = info.lf_pos;
    auto num_inserted = info.num_inserted;
    auto s_rend = std::rbegin(info.s) + num_inserted - 1;
    auto const &wm = accessor_factory::template create<host_type>().get_wm(this);

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
    accessor_factory::template create<host_type>().notify(this,
        typename lcp_trait::event::template after_inserting_lcp<Sequence>{
            info.s, info.num_inserted,
            pos, psi_lcp_, lcpa_it ? *lcpa_it : 0
        });
}

template <typename TI, typename AF>
template <typename Event>
inline void with_lcp<TI, AF>::update(Event const &) {
    // do nothing
}

}  // namespace dict

#endif  // DICT_WITH_LCP_HPP_
