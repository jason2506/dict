/************************************************
 *  lcp_trait.hpp
 *  DESA
 *
 *  Copyright (c) 2015-2016, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_INTERNAL_LCP_TRAIT_HPP_
#define DESA_INTERNAL_LCP_TRAIT_HPP_

namespace desa
{

namespace internal
{

/************************************************
 * Declaration: struct lcp_trait<T>
 ************************************************/

template <typename Trait>
struct lcp_trait
{
    using size_type = typename Trait::size_type;

    struct event
    {
        template <typename Sequence>
        struct after_inserting_lcp
        {
            Sequence const &s;
            size_type num_inserted;

            size_type pos;
            size_type lcp;
            size_type lcp_next;
        };
    };
}; // class lcp_trait<T>

} // namespace internal

} // namespace desa

#endif // DESA_INTERNAL_LCP_TRAIT_HPP_
