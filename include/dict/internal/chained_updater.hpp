/************************************************
 *  chained_updater.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_INTERNAL_CHAINED_UPDATER_HPP_
#define DICT_INTERNAL_CHAINED_UPDATER_HPP_

#include <utility>

namespace dict {

namespace internal {

/************************************************
 * Declaration: struct chained_updater<Us...>
 ************************************************/

template <template <typename...> class... Updaters>
struct chained_updater;

/************************************************
 * Declaration: struct chained_updater<U, Us...>
 ************************************************/

template<
    template <typename...> class FirstUpdater,
    template <typename...> class... RestUpdaters
>
struct chained_updater<FirstUpdater, RestUpdaters...> {
    template <typename... Args>
    class updater;
};  // class chained_updater<U, Us...>

/************************************************
 * Declaration: struct chained_updater<>
 ************************************************/

template <>
struct chained_updater<> {
    template <typename... Args>
    class updater;
};  // class chained_updater<>

/************************************************
 * Declaration: class chained_updater<U, Us...>::updater<Args...>
 ************************************************/

template<
    template <typename...> class FirstUpdater,
    template <typename...> class... RestUpdaters
>
template <typename... Args>
class chained_updater<FirstUpdater, RestUpdaters...>::updater
    : public FirstUpdater<Args...>
    , public chained_updater<RestUpdaters...>::template updater<Args...> {
 public:  // Public Method(s)
    template <typename... ConstructorArgs>
    updater(ConstructorArgs const &... args);

 protected:  // Protected Method(s)
    template <typename Event>
    void update(Event info);

 private:  // Private Type(s)
    using first_updater = FirstUpdater<Args...>;
    using rest_updaters = typename chained_updater<RestUpdaters...>::template updater<Args...>;
};  // class chained_updater<U, Us...>::updater<Args...>

/************************************************
 * Declaration: class chained_updater<>::updater<Args...>
 ************************************************/

template <typename... Args>
class chained_updater<>::updater {
 public:  // Public Method(s)
    template <typename... ConstructorArgs>
    updater(ConstructorArgs const &... args);

 protected:  // Protected Method(s)
    template <typename Event>
    void update(Event);
};  // class chained_updater<>::updater<Args...>

/************************************************
 * Implementation: class chained_updater<U, Us...>::updater<Args...>
 ************************************************/

template <template <typename...> class U, template <typename...> class... Us>
template <typename... Args>
template <typename... ConstructorArgs>
inline chained_updater<U, Us...>::updater<Args...>::updater(ConstructorArgs const &... args)
    : first_updater(args...) , rest_updaters(args...) {
    // do nothing
}

template <template <typename...> class U, template <typename...> class... Us>
template <typename... Args>
template <typename Event>
inline void chained_updater<U, Us...>::updater<Args...>::update(Event info) {
    first_updater::update(info);
    rest_updaters::update(info);
}

/************************************************
 * Implementation: class chained_updater<>::updater<Args...>
 ************************************************/

template <typename... Args>
template <typename... ConstructorArgs>
inline chained_updater<>::updater<Args...>::updater(ConstructorArgs const &... args) {
    // do nothing
}

template <typename... Args>
template <typename Event>
inline void chained_updater<>::updater<Args...>::update(Event) {
    // do nothing
}

}  // namespace internal

}  // namespace dict

#endif  // DICT_INTERNAL_CHAINED_UPDATER_HPP_
