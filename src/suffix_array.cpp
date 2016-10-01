/************************************************
 *  suffix_array.cpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#include "suffix_array.hpp"

namespace desa
{

namespace impl
{

/************************************************
 * Implementation: class suffix_array
 ************************************************/

suffix_array::value_type suffix_array::at(size_type i) const
{
    size_type off = 0;
    while (!sa_samples_[i])
    {
        i = lf(i);
        ++off;
    }

    auto r = sa_samples_.rank(i, true);
    auto j = pi_.at(r - 1);

    auto sa = isa_samples_.select(j, true) + off;
    return sa < size() ? sa : sa - size();
}

suffix_array::size_type suffix_array::rank(value_type j) const
{
    auto br_pair = isa_samples_.access_and_rank(j, true);
    auto b = br_pair.first;
    auto r = br_pair.second;
    if (b)
    {
        // retrieve a value at a sampled position
        auto i = pi_.rank(r - 1);
        return sa_samples_.select(i, true);
    }
    else
    {
        // retrieve a value at a unsampled position
        auto off = isa_samples_.select(r, true) - j;
        auto i = pi_.rank(r);
        auto v = sa_samples_.select(i, true);
        for (decltype(off) t = 0; t < off; ++t)
        {
            v = lf(v);
        }

        return v;
    }
}

void suffix_array::add_samples(value_type j)
{
    if (j + 1 == size()) { return; }

    auto r = isa_samples_.rank(j, true);
    auto left_sample_pos = r > 0 ? isa_samples_.select(r - 1, true) + 1 : 0;
    auto right_sample_pos = isa_samples_.select(r, true);

    auto p = right_sample_pos;
    while (p > left_sample_pos + MAX_SAMPLE_DISTANCE)
    {
        p -= MAX_SAMPLE_DISTANCE + 1;

        auto i = rank(p);
        auto k = i > 0 ? sa_samples_.rank(i - 1, true) : 0;
        sa_samples_.set(i);
        isa_samples_.set(p);
        pi_.insert(k, r);
    }
}

void suffix_array::insert_lcp(size_type kp, size_type psi_kp, size_type lf_kp, size_type &lcp)
{
    auto psi = [&](size_type x) {
        return x == 0 ? kp : wt_.psi(x - (x < lf_kp));
    };

    auto term_at_f = [&](size_type x) {
        return x == 0 ? 0 : wt_.search(x + 1 - (x < lf_kp));
    };

    // calculate LCP[kp]
    auto lcpa_it = lcpa_.find(kp);
    auto old_lcp = lcpa_it ? *lcpa_it : 0;
    if (kp > 0 && psi_kp > 0 && psi(kp - 1) == psi_kp - 1)
    {
        auto c = term_at_f(kp);
        if (c != 0 && c == term_at_f(kp - 1))   { ++lcp; }
        else                                    { lcp = 0; }
    }
    else
    {
        auto x = kp, y = kp - 1;
        for (lcp = 0; lcp < old_lcp; ++lcp)
        {
            x = psi(x);
            y = psi(y);
        }

        auto c = term_at_f(x);
        while (c != 0 && c == term_at_f(y))
        {
            x = psi(x);
            y = psi(y);
            c = term_at_f(x);
            ++lcp;
        }
    }

    if (lcpa_it && old_lcp == lcp)
    {
        // re-calculate LCP[kp + 1]
        auto &lcp = *lcpa_it;
        auto x = kp + 1, y = kp;
        for (lcp = 0; lcp < old_lcp; ++lcp)
        {
            x = psi(x);
            y = psi(y);
        }

        auto c = term_at_f(x);
        while (c != 0 && c == term_at_f(y))
        {
            x = psi(x);
            y = psi(y);
            c = term_at_f(x);
            ++lcp;
        }
    }

    lcpa_.insert(lcpa_it, lcp);
}

} // namespace impl

} // namespace desa
