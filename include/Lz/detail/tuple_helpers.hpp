#pragma once

#ifndef LZ_TUPLE_HELPERS_HPP
#define LZ_TUPLE_HELPERS_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/traits.hpp>
#include <tuple>

namespace lz {
namespace detail {

template<class Tuple>
struct first_it_helper;

template<class Iterator, class... Iterators>
struct first_it_helper<std::tuple<Iterator, Iterators...>> {
    using type = Iterator;
};

template<class Iterator, std::size_t N>
struct first_it_helper<std::array<Iterator, N>> {
    using type = Iterator;
};

template<class Iterators>
using first_it = typename first_it_helper<Iterators>::type;

template<class T>
using canonicalize_reference = conditional<std::is_lvalue_reference<T>::value, T, remove_ref<T>>;

// To decrease the usage of std::tuple, we try to use std::array whenever possible
// Therefore: if all of the iterator/iterables/value/references are homogeneous, we can use std::array

template<class T, class... Ts>
struct maybe_homogenous_helper {
    using type = conditional<conjunction<std::is_same<T, Ts>...>::value, std::array<T, sizeof...(Ts) + 1>, std::tuple<T, Ts...>>;
};

template<class... Ts>
struct maybe_homogenous_helper<std::tuple<Ts...>> {
    using type = typename maybe_homogenous_helper<Ts...>::type;
};

template<class T, std::size_t N>
struct maybe_homogenous_helper<std::array<T, N>> {
    using type = std::array<T, N>;
};

template<class... Ts>
using maybe_homogeneous = typename maybe_homogenous_helper<Ts...>::type;

// To create either a tuple or a an array of values. Canonicalize to prevent rvalue references
template<class T, std::size_t... Is>
maybe_homogeneous<canonicalize_reference<decltype((void)Is, std::declval<T>())>...> make_homogeneous_of(index_sequence<Is...>) {
    return { decltype((void)Is, T{}){}... };
}

// To prevent rvalue references from happening in a tuple, use canonicalize_reference
template<class T, std::size_t... Is>
std::tuple<canonicalize_reference<decltype((void)Is, std::declval<T>())>...> tuple_of(index_sequence<Is...>);

template<class>
struct iter_tuple_diff_type_helper;

template<class... Iterators>
struct iter_tuple_diff_type_helper<std::tuple<Iterators...>> {
    using type = common_type<diff_type<Iterators>...>;
};

template<class Iterator, std::size_t N>
struct iter_tuple_diff_type_helper<std::array<Iterator, N>> {
    using type = diff_type<Iterator>;
};

template<class>
struct iter_tuple_iter_cat_helper;

template<class... Iterators>
struct iter_tuple_iter_cat_helper<std::tuple<Iterators...>> {
    using type = common_type<iter_cat_t<Iterators>...>;
};

template<class Iterator, std::size_t N>
struct iter_tuple_iter_cat_helper<std::array<Iterator, N>> {
    using type = iter_cat_t<Iterator>;
};

template<class>
struct iter_tuple_value_type_helper;

template<class... Iterators>
struct iter_tuple_value_type_helper<std::tuple<Iterators...>> {
    using type = std::tuple<val_t<Iterators>...>;
};

template<class Iterator, std::size_t N>
struct iter_tuple_value_type_helper<std::array<Iterator, N>> {
    using type = decltype(tuple_of<val_t<Iterator>>(make_index_sequence<N>{}));
};

template<class>
struct iter_tuple_ref_type_helper;

template<class... Iterators>
struct iter_tuple_ref_type_helper<std::tuple<Iterators...>> {
    using type = std::tuple<ref_t<Iterators>...>;
};

template<class Iterator, std::size_t N>
struct iter_tuple_ref_type_helper<std::array<Iterator, N>> {
    // Use tuple_of references to avoid array of references, maybe, if ever, use std::reference_wrapper
    using type = decltype(tuple_of<ref_t<Iterator>>(make_index_sequence<N>{}));
};

template<class... Ts>
struct disjunction : std::false_type {};

template<class T, class... Ts>
struct disjunction<T, Ts...> : conditional<T::value, std::true_type, disjunction<Ts...>> {};

template<class>
struct iter_tuple_common_ref_helper;

#ifdef LZ_HAS_CXX_20

template<class... Iterators>
struct iter_tuple_common_ref_helper<std::tuple<Iterators...>> {
    using type = std::common_reference_t<ref_t<Iterators>...>;
};

template<class Iterator, std::size_t N>
struct iter_tuple_common_ref_helper<std::array<Iterator, N>> {
    using type = ref_t<Iterator>;
};

#else

template<class T, class U>
struct copy_cv_helper {
    using const_applied = conditional<std::is_const<T>::value, typename std::add_const<U>::type, U>;
    using type = conditional<std::is_volatile<T>::value, typename std::add_volatile<const_applied>, const_applied>;
};

template<class T, class U>
using copy_cv = typename copy_cv_helper<T, U>::type;

template<class T, class U>
struct common_reference2_helper {
    template<class X, class Y>
    static typename std::add_lvalue_reference<copy_cv<remove_ref<X>, remove_ref<Y>>>::type test(X&, Y&);

    static typename std::common_type<T, U>::type test(...);

    using type = decltype(test(std::declval<T>(), std::declval<U>()));
};

template<class T, class U>
using common_reference2 = typename common_reference2_helper<T, U>::type;

template<class... Ts>
struct common_reference_helper;

template<class T>
struct common_reference_helper<T> {
    using type = T;
};

template<class T1, class T2, class... Ts>
struct common_reference_helper<T1, T2, Ts...> {
    using type = typename common_reference_helper<common_reference2<T1, T2>, Ts...>::type;
};

template<class Iterator, std::size_t N>
struct common_reference_helper<std::array<Iterator, N>> {
    using type = ref_t<Iterator>;
};

template<class... Ts>
using common_reference = typename common_reference_helper<Ts...>::type;

template<class>
struct iter_tuple_common_ref_helper;

template<class... Iterators>
struct iter_tuple_common_ref_helper<std::tuple<Iterators...>> {
    using type = common_reference<ref_t<Iterators>...>;
};

template<class Iterator, std::size_t I>
struct iter_tuple_common_ref_helper<std::array<Iterator, I>> {
    using type = ref_t<Iterator>;
};

#endif

template<class IterTuple>
using iter_tuple_diff_type_t = typename iter_tuple_diff_type_helper<IterTuple>::type;

template<class IterTuple>
using iter_tuple_iter_cat_t = typename iter_tuple_iter_cat_helper<IterTuple>::type;

template<class IterTuple>
using iter_tuple_value_type_t = typename iter_tuple_value_type_helper<IterTuple>::type;

template<class IterTuple>
using iter_tuple_ref_type_t = typename iter_tuple_ref_type_helper<IterTuple>::type;

template<class IterTuple>
using iter_tuple_common_ref_t = typename iter_tuple_common_ref_helper<IterTuple>::type;

template<class Fn>
class tuple_expand {
    Fn _fn;

public:
    template<class F>
    constexpr tuple_expand(F&& fn) : _fn(std::forward<F>(fn)) {
    }

private:
    template<class Tuple, std::size_t... I>
    LZ_CONSTEXPR_CXX_14 auto call(Tuple&& tuple, index_sequence<I...>)
        -> decltype(_fn(std::get<I>(std::forward<Tuple>(tuple))...)) {
        return _fn(std::get<I>(std::forward<Tuple>(tuple))...);
    }

    template<class Tuple, std::size_t... I>
    LZ_CONSTEXPR_CXX_14 auto call(Tuple&& tuple, index_sequence<I...>) const
        -> decltype(_fn(std::get<I>(std::forward<Tuple>(tuple))...)) {
        return _fn(std::get<I>(std::forward<Tuple>(tuple))...);
    }

public:
    template<class Tuple>
    LZ_CONSTEXPR_CXX_14 auto operator()(Tuple&& tuple)
        -> decltype(call(std::forward<Tuple>(tuple), make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>{})) {
        return call(std::forward<Tuple>(tuple), make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>{});
    }

    template<class Tuple>
    LZ_CONSTEXPR_CXX_14 auto operator()(Tuple&& tuple) const
        -> decltype(call(std::forward<Tuple>(tuple), make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>{})) {
        return call(std::forward<Tuple>(tuple), make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>{});
    }
};

template<class Fn>
constexpr tuple_expand<decay_t<Fn>> make_expand_fn(Fn&& fn) {
    return { std::forward<Fn>(fn) };
}

template<class Iterable>
LZ_CONSTEXPR_CXX_14 iter_t<Iterable>
increment_or_decrement_iterator(Iterable&& iterable, std::size_t this_size, std::size_t min) {
    using diff_type = diff_iterable_t<Iterable>;
    // Check whether we can better use decrement or increment. For random access iterators, we can
    // use either, but for bidirectional iterators, we need to be "careful" about the direction.
    // We want to use the iterator that requires the least amount of increments/decrements.
    const auto to_decrement = static_cast<diff_type>(this_size - min);
    const auto to_increment = static_cast<diff_type>(min);
    if (to_increment > to_decrement) {
        return std::prev(detail::end(std::forward<Iterable>(iterable)), to_decrement);
    }
    return std::next(detail::begin(std::forward<Iterable>(iterable)), to_increment);
}

template<std::size_t... Is, class Iterable>
LZ_CONSTEXPR_CXX_14 auto
smallest_end_maybe_homo(Iterable&& iterable_tuple,
                        index_sequence<Is...>) -> maybe_homogeneous<decltype(std::begin(std::get<Is>(iterable_tuple)))...> {
    const std::size_t sizes[] = { static_cast<std::size_t>(lz::eager_size(std::get<Is>(iterable_tuple)))... };
    const auto min = std::min({ sizes[Is]... });

    return { increment_or_decrement_iterator(std::get<Is>(std::forward<Iterable>(iterable_tuple)), sizes[Is], min)... };
}

template<class T, class IterableTuple, std::size_t... Is>
LZ_CONSTEXPR_CXX_14 maybe_homogeneous<decltype(Is, T{})...>
iterable_maybe_homo_eager_size_as(IterableTuple&& iterables, index_sequence<Is...>) {
    return { static_cast<T>(lz::eager_size(std::get<Is>(std::forward<IterableTuple>(iterables))))... };
}

template<class IterableTuple, std::size_t... I>
LZ_CONSTEXPR_CXX_14 auto begin_maybe_homo_impl(IterableTuple&& iterable_tuple, index_sequence<I...>)
    -> maybe_homogeneous<decltype(detail::begin(std::get<I>(std::forward<IterableTuple>(iterable_tuple))))...> {
    return { detail::begin(std::get<I>(std::forward<IterableTuple>(iterable_tuple)))... };
}

template<class IterableTuple, std::size_t... I>
LZ_CONSTEXPR_CXX_14 auto end_maybe_homo_impl(IterableTuple&& iterable_tuple, index_sequence<I...>)
    -> maybe_homogeneous<decltype(detail::end(std::get<I>(std::forward<IterableTuple>(iterable_tuple))))...> {
    return { detail::end(std::get<I>(std::forward<IterableTuple>(iterable_tuple)))... };
}

template<class IterableTuple>
LZ_CONSTEXPR_CXX_14 auto begin_maybe_homo(IterableTuple&& iterable_tuple)
    -> decltype(begin_maybe_homo_impl(std::forward<IterableTuple>(iterable_tuple),
                                      make_index_sequence<std::tuple_size<remove_cvref<IterableTuple>>::value>{})) {
    return begin_maybe_homo_impl(std::forward<IterableTuple>(iterable_tuple),
                                 make_index_sequence<std::tuple_size<remove_cvref<IterableTuple>>::value>{});
}

template<class IterableTuple>
LZ_CONSTEXPR_CXX_14 auto end_maybe_homo(IterableTuple&& iterable_tuple)
    -> decltype(end_maybe_homo_impl(std::forward<IterableTuple>(iterable_tuple),
                                    make_index_sequence<std::tuple_size<remove_cvref<IterableTuple>>::value>{})) {
    return end_maybe_homo_impl(std::forward<IterableTuple>(iterable_tuple),
                               make_index_sequence<std::tuple_size<remove_cvref<IterableTuple>>::value>{});
}
} // namespace detail
} // namespace lz

#endif
