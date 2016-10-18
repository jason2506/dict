/************************************************
 *  updating_nothing_policy.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_UPDATING_NOTHING_POLICY_HPP_
#define DESA_UPDATING_NOTHING_POLICY_HPP_

#include "wavelet_tree.hpp"

namespace desa
{

namespace impl
{

/************************************************
 * Declaration: class updating_nothing_policy<TI>
 ************************************************/

template <typename TextIndex>
class updating_nothing_policy
{
public: // Public Type(s)
    typedef ::std::size_t size_type;
    typedef ::std::uint16_t term_type;

public: // Public Method(s)
    updating_nothing_policy(wavelet_tree<term_type> const &wt);

protected: // Protected Method(s)
    void update_after_inserting_first_term(void);
    void update_after_inserting_term(size_type kp, size_type psi_kp, size_type lf_kp);
    void update_after_inserting_sequence(void);
}; // class updating_nothing_policy<TI>

/************************************************
 * Implementation: class updating_nothing_policy<TI>
 ************************************************/

template <typename TI>
inline updating_nothing_policy<TI>::updating_nothing_policy(wavelet_tree<term_type> const &wt)
{
    // do nothing
}

template <typename TI>
inline void updating_nothing_policy<TI>::update_after_inserting_first_term(void)
{
    // do nothing
}

template <typename TI>
void updating_nothing_policy<TI>::update_after_inserting_term(size_type kp, size_type psi_kp, size_type lf_kp)
{
    // do nothing
}

template <typename TI>
inline void updating_nothing_policy<TI>::update_after_inserting_sequence(void)
{
    // do nothing
}

} // namespace impl

} // namespace desa

#endif // DESA_UPDATING_NOTHING_POLICY_HPP_
