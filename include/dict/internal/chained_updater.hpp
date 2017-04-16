/************************************************
 *  chained_updater.hpp
 *  DICT
 *
 *  Copyright (c) 2015-2017, Chi-En Wu
 *  Distributed under The BSD 3-Clause License
 ************************************************/

#ifndef DICT_INTERNAL_CHAINED_UPDATER_HPP_
#define DICT_INTERNAL_CHAINED_UPDATER_HPP_

namespace dict {

namespace internal {

/************************************************
 * Declaration: class chained_updater<T, Us...>
 ************************************************/

template <typename UpdaterArgs, template <typename...> class... Updaters>
class chained_updater;

/************************************************
 * Declaration: class chained_updater<T, U, Us...>
 ************************************************/

template <
    typename UpdaterArgs,
    template <typename...> class FirstUpdater,
    template <typename...> class... RestUpdaters
>
class chained_updater<UpdaterArgs, FirstUpdater, RestUpdaters...>
    : public UpdaterArgs::template apply<FirstUpdater>
    , public chained_updater<UpdaterArgs, RestUpdaters...> {
 public:  // Public Method(s)
    template <typename... Args>
    explicit chained_updater(Args const &... args);

 protected:  // Protected Method(s)
    template <typename Event>
    void update(Event const &info);

 private:  // Private Type(s)
    using first_updater = typename UpdaterArgs::template apply<FirstUpdater>;
    using rest_updaters = chained_updater<UpdaterArgs, RestUpdaters...>;
};  // class chained_updater<T, U, Us...>

/************************************************
 * Declaration: class chained_updater<T>
 ************************************************/

template <typename UpdaterArgs>
class chained_updater<UpdaterArgs> {
 public:  // Public Method(s)
    template <typename... Args>
    explicit chained_updater(Args const &... args);

 protected:  // Protected Method(s)
    template <typename Event>
    void update(Event const &info);
};  // class chained_updater<T>

/************************************************
 * Implementation: class chained_updater<T, U, Us...>
 ************************************************/

template <
    typename UpdaterArgs,
    template <typename...> class U,
    template <typename...> class... Us
>
template <typename... Args>
inline chained_updater<UpdaterArgs, U, Us...>::chained_updater(Args const &... args)
    : first_updater(args...) , rest_updaters(args...) {
    // do nothing
}

template <
    typename UpdaterArgs,
    template <typename...> class U,
    template <typename...> class... Us
>
template <typename Event>
inline void chained_updater<UpdaterArgs, U, Us...>::update(Event const &info) {
    first_updater::update(info);
    rest_updaters::update(info);
}

/************************************************
 * Implementation: class chained_updater<T>
 ************************************************/

template <typename UpdaterArgs>
template <typename... Args>
inline chained_updater<UpdaterArgs>::chained_updater(Args const &... args) {
    // do nothing
}

template <typename UpdaterArgs>
template <typename Event>
inline void chained_updater<UpdaterArgs>::update(Event const &) {
    // do nothing
}

}  // namespace internal

}  // namespace dict

#endif  // DICT_INTERNAL_CHAINED_UPDATER_HPP_
