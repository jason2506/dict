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
 * Declaration: class chained_updater<TI, AF, Us...>
 ************************************************/

template <
    typename TextIndex,
    typename AccessorFactory,
    template <typename, typename> class... Updaters
>
class chained_updater;

/************************************************
 * Declaration: class chained_updater<TI, AF, U, Us...>
 ************************************************/

template <
    typename TextIndex,
    typename AccessorFactory,
    template <typename, typename> class FirstUpdater,
    template <typename, typename> class... RestUpdaters
>
class chained_updater<TextIndex, AccessorFactory, FirstUpdater, RestUpdaters...>
    : public FirstUpdater<TextIndex, AccessorFactory>
    , public chained_updater<TextIndex, AccessorFactory, RestUpdaters...> {
 public:  // Public Method(s)
    template <typename... Args>
    explicit chained_updater(Args const &... args);

 protected:  // Protected Method(s)
    template <typename Event>
    void update(Event const &info);

 private:  // Private Type(s)
    using first_updater = FirstUpdater<TextIndex, AccessorFactory>;
    using rest_updaters = chained_updater<TextIndex, AccessorFactory, RestUpdaters...>;
};  // class chained_updater<TI, AF, U, Us...>

/************************************************
 * Declaration: class chained_updater<TI, AF>
 ************************************************/

template <typename TextIndex, typename AccessorFactory>
class chained_updater<TextIndex, AccessorFactory> {
 public:  // Public Method(s)
    template <typename... Args>
    explicit chained_updater(Args const &... args);

 protected:  // Protected Method(s)
    template <typename Event>
    void update(Event const &info);
};  // class chained_updater<TI, AF>

/************************************************
 * Implementation: class chained_updater<TI, AF, U, Us...>
 ************************************************/

template <
    typename TI,
    typename AF,
    template <typename, typename> class U,
    template <typename, typename> class... Us
>
template <typename... Args>
inline chained_updater<TI, AF, U, Us...>::chained_updater(Args const &... args)
    : first_updater(args...) , rest_updaters(args...) {
    // do nothing
}

template <
    typename TI,
    typename AF,
    template <typename, typename> class U,
    template <typename, typename> class... Us
>
template <typename Event>
inline void chained_updater<TI, AF, U, Us...>::update(Event const &info) {
    first_updater::update(info);
    rest_updaters::update(info);
}

/************************************************
 * Implementation: class chained_updater<TI, AF>
 ************************************************/

template <typename TI, typename AF>
template <typename... Args>
inline chained_updater<TI, AF>::chained_updater(Args const &... args) {
    // do nothing
}

template <typename TI, typename AF>
template <typename Event>
inline void chained_updater<TI, AF>::update(Event const &) {
    // do nothing
}

}  // namespace internal

}  // namespace dict

#endif  // DICT_INTERNAL_CHAINED_UPDATER_HPP_
