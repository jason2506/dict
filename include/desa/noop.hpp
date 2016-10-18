/************************************************
 *  noop.hpp
 *  DESA
 *
 *  Copyright (c) 2015, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DESA_NOOP_HPP_
#define DESA_NOOP_HPP_

namespace desa
{

namespace impl
{

template <typename ...Args>
inline void noop(Args... args)
{
    // do nothing
}

} // namespace impl

} // namespace desa

#endif // DESA_NOOP_HPP_
