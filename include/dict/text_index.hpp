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
#include <utility>

#include "internal/chained_updater.hpp"
#include "internal/text_index_trait.hpp"
#include "internal/type_list.hpp"

namespace dict {

/************************************************
 * Declaration: class text_index<UPs...>
 ************************************************/

template <template <typename, typename> class... UpdatingPolicies>
class text_index : public internal::chained_updater<
    internal::type_list<
        text_index<UpdatingPolicies...>,
        internal::text_index_trait
    >,
    UpdatingPolicies...
> {
 public:  // Public Type(s)
    using size_type = internal::text_index_trait::size_type;
    using term_type = internal::text_index_trait::term_type;
    using seq_type = internal::text_index_trait::seq_type;

 public:  // Public Method(s)
    text_index();

    template <typename Sequence>
    void insert(Sequence const &s);
    size_type erase(size_type i);

    template <typename OutputIterator>
    std::pair<size_type, OutputIterator>
    reverse_recover(size_type i, OutputIterator it) const;

    bool empty() const;
    size_type num_seqs() const;
    size_type num_terms() const;

    term_type f(size_type i) const;
    term_type bwt(size_type i) const;
    size_type psi(size_type i) const;
    size_type lf(size_type i) const;

 private:  // Private Type(s)
    friend internal::text_index_trait::helper;

    using wm_type = internal::text_index_trait::wm_type;
    using event = internal::text_index_trait::event;
    using updating_policies = internal::chained_updater<
            internal::type_list<
                text_index<UpdatingPolicies...>,
                internal::text_index_trait
            >,
            UpdatingPolicies...
        >;

 private:  // Private Method(s)
    size_type reorder(size_type actual, size_type expected);

 private:  // Private Property(ies)
    wm_type wm_;
    size_type sentinel_pos_;
    size_type sentinel_rank_;
    size_type num_seqs_;
};  // class text_index<UPs...>

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
template <typename OutputIterator>
std::pair<typename text_index<UPs...>::size_type, OutputIterator>
text_index<UPs...>::reverse_recover(size_type i, OutputIterator it) const {
    assert(f(i) == 0);

    i = lf(i);
    auto c = f(i);
    do {
        *it++ = c;
        i = lf(i);
        c = f(i);
    } while (c != 0);

    return {i, it};
}

template <template <typename, typename> class... UPs>
inline typename text_index<UPs...>::size_type text_index<UPs...>::erase(size_type k) {
    auto i = psi(k);
    assert(wm_[i] == 0);

    updating_policies::update(event::before_erasuring_sequence{k});
    seq_type s;

    decltype(wm_.access_and_lf(0)) c_lf_pair(0, wm_.lf(k));
    do {
        auto c = wm_.erase(k);
        s.push_back(c);
        updating_policies::update(event::after_erasuring_term{s, k});

        assert(i != k);
        if (i > k) { --i; }
        if (sentinel_pos_ > k) { --sentinel_pos_; }

        auto lf_k = c_lf_pair.second - 1;
        k = lf_k;
        c_lf_pair = wm_.access_and_lf(k);

        assert(wm_[i] == 0);
    } while (c_lf_pair.first != 0);

    auto c = wm_.erase(k);
    assert(c == 0);
    s.push_back(c);
    updating_policies::update(event::after_erasuring_term{s, k});

    if (wm_.size()) {
        assert(num_seqs_ > 1);
        assert(i != k);
        if (i > k) { --i; }

        assert(wm_[i] == 0);
        if (sentinel_pos_ == k) {
            // erased sequence is the first one
            sentinel_pos_ = i;
        } else {
            auto lf_k = c_lf_pair.second - (i < k);
            if (k < sentinel_pos_) {
                --sentinel_pos_;
                ++lf_k;
            }

            if (i == sentinel_pos_ && i >= k) {
                --lf_k;
            }

            assert(wm_[sentinel_pos_] == 0);
            i = reorder(lf_k, lf(i));
            assert(f(i) == 0);
        }

        assert(wm_[sentinel_pos_] == 0);
        sentinel_rank_ = wm_.rank(sentinel_pos_, 0);
    } else {
        // no more sequences remain
        assert(num_seqs_ == 1);
        i = sentinel_pos_ = sentinel_rank_ = 0;
    }

    --num_seqs_;

    updating_policies::update(event::after_erasuring_sequence{s});
    return i;
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

template <template <typename, typename> class... UPs>
typename text_index<UPs...>::size_type
text_index<UPs...>::reorder(size_type actual, size_type expected) {
    auto i = expected;
    while (actual != expected) {
        auto k = lf(actual);
        auto c = wm_.erase(actual);
        wm_.insert(expected, c);

        if (actual > sentinel_pos_ && expected <= sentinel_pos_) {
            ++sentinel_pos_;
        } else if (actual < sentinel_pos_ && expected >= sentinel_pos_) {
            --sentinel_pos_;
        } else if (actual == sentinel_pos_) {
            sentinel_pos_ = expected;
        }

        if (actual > i && expected < i) {
            ++i;
        } else if (actual < i && expected > i) {
            --i;
        } else if (i == actual) {
            i = expected;
        }

        updating_policies::update(event::after_moving_term{actual, expected});

        actual = k;
        expected = lf(expected);
    }

    return i;
}

}  // namespace dict

#endif  // DICT_TEXT_INDEX_HPP_
