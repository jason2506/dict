/************************************************
 *  with_lcp.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_WITH_LCP_HPP_
#define DICT_WITH_LCP_HPP_

#include "internal/with_lcp_impl.hpp"

namespace dict {

/************************************************
 * Declaration: class with_lcp<UPs...>
 ************************************************/

template <template <typename, typename> class... UpdatingPolicies>
struct with_lcp {
    template <typename TextIndex, typename Trait>
    using policy = internal::with_lcp_impl<TextIndex, Trait, UpdatingPolicies...>;
};  // class with_lcp<UPs...>

}  // namespace dict

#endif  // DICT_WITH_LCP_HPP_
