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

void suffix_array::erase(value_type j, size_type m)
{
    if (m == 0) { return; }

    auto i = rank(j + m);
    auto replaced_term = wt_[i];
    auto k = lf(i);
    term_type c;
    do
    {
        if (k <= i) { --i; }

        --m;
        c = erase_term(k, j + m);
        auto d = 0;
        if (i < k && c == replaced_term) { ++d; }
        if (c > replaced_term) { ++d; }
        k = wt_.sum(c) + wt_.rank(k - 1, c) - d;
    } while (m > 0);

    wt_.erase(i);
    wt_.insert(i, c);

    reorder(k, lf(i));
    add_samples(j);
}

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
    if (isa_samples_[j])
    {
        // retrieve a value at a sampled position
        auto r = isa_samples_.rank(j, true);
        auto i = pi_.rank(r - 1);
        return sa_samples_.select(i, true);
    }
    else
    {
        // retrieve a value at a unsampled position
        auto r = isa_samples_.rank(j, true);
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

void suffix_array::reorder(size_type actual, size_type expected)
{
    while (actual != expected)
    {
        auto k = lf(actual);
        auto c = wt_.erase(actual);
        wt_.insert(expected, c);

        // move sample
        size_type old_rank;
        auto b = sa_samples_.erase(actual);
        if (b)
        {
            old_rank = actual > 0 ? sa_samples_.rank(actual - 1, true) : 0;
        }

        sa_samples_.insert(expected, b);
        if (b)
        {
            auto new_rank = sa_samples_.rank(expected, true) - 1;
            if (old_rank != new_rank)
            {
                pi_.move(old_rank, new_rank);
            }
        }

        actual = k;
        expected = lf(expected);
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

} // namespace impl

} // namespace desa
