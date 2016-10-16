/************************************************
 *  chained_updating_policy.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_CHAINED_UPDATING_POLICY_HPP_
#define DESA_CHAINED_UPDATING_POLICY_HPP_

#include "wavelet_tree.hpp"

namespace desa
{

namespace impl
{

/************************************************
 * Declaration: class chained_updating_policy<TI, FUP, SUP>
 ************************************************/

template
<
    typename TextIndex,
    template <typename> class FirstUpdatingPolicy,
    template <typename> class SecondUpdatingPolicy
>
class chained_updating_policy
    : public FirstUpdatingPolicy<TextIndex>
    , public SecondUpdatingPolicy<TextIndex>
{
public: // Public Type(s)
    typedef ::std::size_t size_type;
    typedef ::std::uint16_t term_type;

public: // Public Method(s)
    chained_updating_policy(wavelet_tree<term_type> const &wt);

protected: // Protected Method(s)
    void update_after_inserting_first_term(void);
    void update_after_inserting_term(size_type kp, size_type psi_kp, size_type lf_kp);
    void update_after_inserting_sequence(void);

private: // Private Type(s)
    typedef FirstUpdatingPolicy<TextIndex> first_policy_type;
    typedef SecondUpdatingPolicy<TextIndex> second_policy_type;
}; // class chained_updating_policy<TI, FUP, SUP>

/************************************************
 * Implementation: class chained_updating_policy<TI, FUP, SUP>
 ************************************************/

template <typename TI, template <typename> class FUP, template <typename> class SUP>
inline chained_updating_policy<TI, FUP, SUP>::chained_updating_policy(wavelet_tree<term_type> const &wt)
    : first_policy_type(wt), second_policy_type(wt)
{
    // do nothing
}

template <typename TI, template <typename> class FUP, template <typename> class SUP>
inline void chained_updating_policy<TI, FUP, SUP>::update_after_inserting_first_term(void)
{
    first_policy_type::update_after_inserting_first_term();
    second_policy_type::update_after_inserting_first_term();
}

template <typename TI, template <typename> class FUP, template <typename> class SUP>
inline void chained_updating_policy<TI, FUP, SUP>::update_after_inserting_term(size_type kp, size_type psi_kp, size_type lf_kp)
{
    first_policy_type::update_after_inserting_term(kp, psi_kp, lf_kp);
    second_policy_type::update_after_inserting_term(kp, psi_kp, lf_kp);
}

template <typename TI, template <typename> class FUP, template <typename> class SUP>
inline void chained_updating_policy<TI, FUP, SUP>::update_after_inserting_sequence(void)
{
    first_policy_type::update_after_inserting_sequence();
    second_policy_type::update_after_inserting_sequence();
}

} // namespace impl

} // namespace desa

#endif // DESA_CHAINED_UPDATING_POLICY_HPP_
