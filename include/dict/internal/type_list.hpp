/************************************************
 *  type_list.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_INTERNAL_TYPE_LIST_HPP_
#define DICT_INTERNAL_TYPE_LIST_HPP_

namespace dict {

namespace internal {

/************************************************
 * Declaration: struct type_list<Ts...>
 ************************************************/

template <typename... Ts>
struct type_list {
    template <template <typename...> class U>
    using apply = U<Ts...>;
};  // struct type_list<Ts...>

}  // namespace internal

}  // namespace dict

#endif  // DICT_INTERNAL_TYPE_LIST_HPP_
