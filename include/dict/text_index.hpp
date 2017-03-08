/************************************************
 *  text_index.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_TEXT_INDEX_HPP_
#define DICT_TEXT_INDEX_HPP_

#include <cassert>

#include <iterator>

#include "internal/chained_updater.hpp"
#include "internal/text_index_trait.hpp"

namespace dict {

namespace internal {

struct accessor_factory {
    template <typename T>
    static typename T::accessor create() {
        return decltype(create<T>())();
    }
};

};

using internal::text_index_trait;

/************************************************
 * Declaration: class text_index<UPs...>
 ************************************************/

template <template <typename, typename> class... UpdatingPolicies>
class text_index : public internal::chained_updater<
    text_index<UpdatingPolicies...>,
    internal::accessor_factory,
    UpdatingPolicies...
> {
 public:  // Public Type(s)
    using size_type = text_index_trait::size_type;
    using term_type = text_index_trait::term_type;

 public:  // Public Method(s)
    text_index();

    template <typename Sequence>
    void insert(Sequence const &s);

    bool empty() const;
    size_type num_seqs() const;
    size_type num_terms() const;

    term_type f(size_type i) const;
    term_type bwt(size_type i) const;
    size_type psi(size_type i) const;
    size_type lf(size_type i) const;

 private:  // Private Type(s)
    friend internal::accessor_factory;
    struct accessor;

    using wm_type = text_index_trait::wm_type;
    using event = text_index_trait::event;
    using updating_policies = internal::chained_updater<
            text_index<UpdatingPolicies...>,
            internal::accessor_factory,
            UpdatingPolicies...
        >;

 private:  // Private Property(ies)
    wm_type wm_;
    size_type sentinel_pos_;
    size_type sentinel_rank_;
    size_type num_seqs_;
};  // class text_index<UPs...>

/************************************************
 * Declaration: class text_index<UPs...>::accessor
 ************************************************/

template <template <typename, typename> class... UPs>
struct text_index<UPs...>::accessor {
    using host_type = text_index<UPs...>;

    template <typename TextIndex>
    host_type *to_host(TextIndex *ti) {  // NOLINT(runtime/references)
        return static_cast<host_type *>(ti);
    }

    template <typename TextIndex>
    host_type const *to_host(TextIndex const *ti) {
        return static_cast<host_type const *>(ti);
    }

    template <typename TextIndex>
    typename host_type::wm_type const &get_wm(TextIndex const *ti) {
        return to_host(ti)->wm_;
    }

    template <typename TextIndex, typename Event>
    void notify(TextIndex *ti, Event const &info) {
        to_host(ti)->update(info);
    }
};  // class text_index<UPs...>::accessor

/************************************************
 * Implementation: class text_index<UPs...>
 ************************************************/

template <template <typename, typename> class... UPs>
inline text_index<UPs...>::text_index()
    : updating_policies(), sentinel_pos_(0), sentinel_rank_(0), num_seqs_(0) {
    // do nothing
}

template <template <typename, typename> class... UPs>
template <typename Sequence>
void text_index<UPs...>::insert(Sequence const &s) {
    auto seq_it = std::rbegin(s);
    auto seq_end = std::rend(s);
    if (seq_it == seq_end) { return; }

    size_type num_inserted = 0;
    decltype(lf(0)) kp, psi_kp;
    if (empty()) {
        // insert first sampled point at last position
        assert(*seq_it != 0);
        wm_.insert(0, *seq_it);

        updating_policies::update(event::after_inserting_first_term<Sequence>{s});

        kp = 1;
        psi_kp = 0;
        ++seq_it;
        ++num_inserted;
    } else {
        psi_kp = sentinel_pos_;
        kp = wm_.lf(sentinel_pos_) + 1;
        if (kp <= psi_kp) { ++psi_kp; }
    }

    while (seq_it != seq_end) {
        assert(*seq_it != 0);
        wm_.insert(kp, *seq_it);

        auto lf_kp = wm_.lf(kp) + 1;
        updating_policies::update(event::after_inserting_term<Sequence>{
            s, num_inserted++,
            kp, psi_kp, lf_kp
        });

        psi_kp = kp;
        kp = lf_kp;
        if (kp <= psi_kp) { ++psi_kp; }

        ++seq_it;
    }

    wm_.insert(kp, 0);

    updating_policies::update(event::after_inserting_term<Sequence>{
        s, num_inserted++,
        kp, psi_kp, 0
    });

    sentinel_pos_ = kp;
    sentinel_rank_ = wm_.rank(kp, 0);
    ++num_seqs_;

    updating_policies::update(event::after_inserting_sequence<Sequence>{s});
}

template <template <typename, typename> class... UPs>
inline bool text_index<UPs...>::empty() const {
    return num_seqs_ == 0;
}

template <template <typename, typename> class... UPs>
inline typename text_index<UPs...>::size_type text_index<UPs...>::num_seqs() const {
    return num_seqs_;
}

template <template <typename, typename> class... UPs>
inline typename text_index<UPs...>::size_type text_index<UPs...>::num_terms() const {
    return wm_.size();
}

template <template <typename, typename> class... UPs>
inline typename text_index<UPs...>::term_type text_index<UPs...>::f(size_type i) const {
    return wm_.search(i + 1);
}

template <template <typename, typename> class... UPs>
inline typename text_index<UPs...>::term_type text_index<UPs...>::bwt(size_type i) const {
    return wm_[i];
}

template <template <typename, typename> class... UPs>
inline typename text_index<UPs...>::size_type text_index<UPs...>::psi(size_type i) const {
    if (i == 0) { return sentinel_pos_; }

    return i < sentinel_rank_
        ? wm_.select(i - 1, 0)
        : wm_.psi(i);
}

template <template <typename, typename> class... UPs>
inline typename text_index<UPs...>::size_type text_index<UPs...>::lf(size_type i) const {
    if (i == sentinel_pos_) { return 0; }

    auto pair = wm_.access_and_lf(i);
    return (pair.first == 0 && i < sentinel_pos_) + pair.second;
}

}  // namespace dict

#endif  // DICT_TEXT_INDEX_HPP_
