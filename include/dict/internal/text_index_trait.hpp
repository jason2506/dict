/************************************************
 *  text_index_trait.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_INTERNAL_TEXT_INDEX_TRAIT_HPP_
#define DICT_INTERNAL_TEXT_INDEX_TRAIT_HPP_

#include "wavelet_matrix.hpp"

namespace dict {

namespace internal {

/************************************************
 * Declaration: struct text_index_trait
 ************************************************/

struct text_index_trait {
    using size_type = std::size_t;
    using term_type = std::uint16_t;
    using wm_type = wavelet_matrix<term_type>;

    struct helper;
    struct event;
};  // class text_index_trait

/************************************************
 * Declaration: struct text_index_trait::helper
 ************************************************/

struct text_index_trait::helper {
    template <typename TextIndex>
    static typename TextIndex::host_type *to_host(TextIndex *ti) {  // NOLINT(runtime/references)
        return static_cast<typename TextIndex::host_type *>(ti);
    }

    template <typename TextIndex>
    static typename TextIndex::host_type const *to_host(TextIndex const *ti) {
        return static_cast<typename TextIndex::host_type const *>(ti);
    }

    template <typename TextIndex>
    static wm_type const &get_wm(TextIndex const *ti) {
        return to_host(ti)->wm_;
    }
};  // class text_index_trait::helper

/************************************************
 * Declaration: struct text_index_trait::event
 ************************************************/

struct text_index_trait::event {
    template <typename Sequence>
    struct after_inserting_first_term {
        Sequence const &s;
    };

    template <typename Sequence>
    struct after_inserting_term {
        Sequence const &s;
        size_type num_inserted;

        size_type pos;
        size_type psi_pos;
        size_type lf_pos;
    };

    template <typename Sequence>
    struct after_inserting_sequence {
        Sequence const &s;
    };
};  // class text_index_trait::event

}  // namespace internal

}  // namespace dict

#endif  // DICT_INTERNAL_TEXT_INDEX_TRAIT_HPP_
