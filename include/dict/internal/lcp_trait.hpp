/************************************************
 *  lcp_trait.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_INTERNAL_LCP_TRAIT_HPP_
#define DICT_INTERNAL_LCP_TRAIT_HPP_

namespace dict {

namespace internal {

/************************************************
 * Declaration: struct lcp_trait<T>
 ************************************************/

template <typename Trait>
struct lcp_trait {
    using size_type = typename Trait::size_type;
    using term_type = typename Trait::term_type;
    using seq_type = typename Trait::seq_type;

    struct event {
        template <typename Sequence>
        struct after_inserting_lcp {
            Sequence const &s;
            size_type num_inserted;

            size_type pos;
            size_type lcp;
            size_type lcp_next;
        };

        struct after_erasing_lcp {
            seq_type const &s;

            size_type pos;
            size_type lcp;
            size_type lcp_next;
        };
    };
};  // class lcp_trait<T>

}  // namespace internal

}  // namespace dict

#endif  // DICT_INTERNAL_LCP_TRAIT_HPP_
