/************************************************
 *  text_index_trait.hpp
 *  DESA
 *
 *  Copyright (c) 2015-2016, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_INTERNAL_TEXT_INDEX_TRAIT_HPP_
#define DESA_INTERNAL_TEXT_INDEX_TRAIT_HPP_

#include "wavelet_tree.hpp"

namespace desa
{

namespace internal
{

/************************************************
 * Declaration: struct text_index_trait
 ************************************************/

struct text_index_trait
{
    using size_type = ::std::size_t;
    using term_type = ::std::uint16_t;
    using wt_type = wavelet_tree<term_type>;

    struct event
    {
        struct after_inserting_first_term
        { /* empty */ };

        struct after_inserting_term
        {
            size_type pos;
            size_type psi_pos;
            size_type lf_pos;
        };

        struct after_inserting_sequence
        { /* empty */ };
    };
}; // class text_index_trait

} // namespace internal

} // namespace desa

#endif // DESA_INTERNAL_TEXT_INDEX_TRAIT_HPP_
