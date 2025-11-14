#pragma once

#ifndef LZ_TUPLE_HELPERS_HPP
#define LZ_TUPLE_HELPERS_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/maybe_owned.hpp>
#include <Lz/detail/traits/conjunction.hpp>
#include <Lz/detail/traits/index_sequence.hpp>
#include <Lz/detail/traits/is_reference_wrapper.hpp>
#include <Lz/procs/eager_size.hpp>
#include <algorithm>
#include <tuple>

namespace lz {
namespace detail {

// To prevent inclusion of <array> in the header, we define a trivial homogeneous array ourselves
template<class T, size_t N>
struct homogeneous_array {
    T array[N];

    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool friend operator==(const homogeneous_array& lhs, const homogeneous_array& rhs) {
        for (size_t i = 0; i < N; ++i) {
            if (!(lhs.array[i] == rhs.array[i])) {
                return false;
            }
        }
        return true;
    }
};

template<size_t I, class T, size_t N>
LZ_NODISCARD constexpr const T& get(const homogeneous_array<T, N>& arr) noexcept {
    static_assert(I < N, "Index out of bounds");
    return arr.array[I];
}

template<size_t I, class T, size_t N>
LZ_NODISCARD constexpr T& get(homogeneous_array<T, N>& arr) noexcept {
    return const_cast<T&>(get<I>(static_cast<const homogeneous_array<T, N>&>(arr)));
}

template<class Tuple>
struct first_it;

template<class Iterator, class... Iterators>
struct first_it<std::tuple<Iterator, Iterators...>> {
    using type = Iterator;
};

template<class Iterator, size_t N>
struct first_it<homogeneous_array<Iterator, N>> {
    using type = Iterator;
};

template<class Iterators>
using first_it_t = typename first_it<Iterators>::type;

template<class Tuple>
struct tuple_size : std::tuple_size<Tuple> {};

template<class Iterator, size_t N>
struct tuple_size<homogeneous_array<Iterator, N>> {
    static constexpr size_t value = N;
};

// To decrease the usage of std::tuple, we try to use homogeneous_array whenever possible
// Therefore: if all of the iterator/iterables/value/references are homogeneous, we can use homogeneous_array

template<class... Ts>
struct maybe_homogenous;

template<>
struct maybe_homogenous<> {
    struct type {};
};

template<class T, class... Ts>
struct maybe_homogenous<T, Ts...> {
    using type =
        conditional_t<conjunction<std::is_same<T, Ts>...>::value, homogeneous_array<T, sizeof...(Ts) + 1>, std::tuple<T, Ts...>>;
};

template<class... Ts>
struct maybe_homogenous<std::tuple<Ts...>> {
    using type = typename maybe_homogenous<Ts...>::type;
};

template<class T, size_t N>
struct maybe_homogenous<homogeneous_array<T, N>> {
    using type = homogeneous_array<T, N>;
};

template<class... Ts>
using maybe_homogeneous_t = typename maybe_homogenous<Ts...>::type;

// To create either a tuple or a an array of values. Canonicalize to prevent rvalue references
template<class T, size_t... Is>
constexpr maybe_homogeneous_t<remove_rvalue_reference_t<decltype((void)Is, std::declval<T>())>...>
make_homogeneous_of(index_sequence<Is...>) {
    return { decltype((void)Is, T{}){}... };
}

// To prevent rvalue references from happening in a tuple, use canonicalize_reference
template<class T, size_t... Is>
constexpr std::tuple<remove_rvalue_reference_t<decltype((void)Is, std::declval<T>())>...> tuple_of(index_sequence<Is...>);

template<class>
struct iter_tuple_diff_type;

template<class... Iterators>
struct iter_tuple_diff_type<std::tuple<Iterators...>> {
    using type = typename std::common_type<diff_type<Iterators>...>::type;
};

template<class Iterator, size_t N>
struct iter_tuple_diff_type<homogeneous_array<Iterator, N>> {
    using type = diff_type<Iterator>;
};

template<class IterTuple>
using iter_tuple_diff_type_t = typename iter_tuple_diff_type<IterTuple>::type;

template<class>
struct iter_tuple_iter_cat;

template<class... Iterators>
struct iter_tuple_iter_cat<std::tuple<Iterators...>> {
    using type = typename std::common_type<iter_cat_t<Iterators>...>::type;
};

template<class Iterator, size_t N>
struct iter_tuple_iter_cat<homogeneous_array<Iterator, N>> {
    using type = iter_cat_t<Iterator>;
};

template<class IterTuple>
using iter_tuple_iter_cat_t = typename iter_tuple_iter_cat<IterTuple>::type;

template<class>
struct iter_tuple_value_type;

template<class... Iterators>
struct iter_tuple_value_type<std::tuple<Iterators...>> {
    using type = std::tuple<val_t<Iterators>...>;
};

template<class Iterator, size_t N>
struct iter_tuple_value_type<homogeneous_array<Iterator, N>> {
    using type = decltype(tuple_of<val_t<Iterator>>(make_index_sequence<N>{}));
};

template<class IterTuple>
using iter_tuple_value_type_t = typename iter_tuple_value_type<IterTuple>::type;

template<class>
struct iter_tuple_ref_type;

template<class... Iterators>
struct iter_tuple_ref_type<std::tuple<Iterators...>> {
    using type = std::tuple<ref_t<Iterators>...>;
};

template<class Iterator, size_t N>
struct iter_tuple_ref_type<homogeneous_array<Iterator, N>> {
    // Use tuple_of references to avoid array of references, maybe, if ever, use std::reference_wrapper
    using type = decltype(tuple_of<ref_t<Iterator>>(make_index_sequence<N>{}));
};

template<class IterTuple>
using iter_tuple_ref_type_t = typename iter_tuple_ref_type<IterTuple>::type;

#ifdef LZ_HAS_CXX_17

template<class... Ts>
struct disjunction : std::bool_constant<(Ts::value || ...)> {};

#else

template<class... Ts>
struct disjunction : std::false_type {};

template<class T, class... Ts>
struct disjunction<T, Ts...> : conditional_t<T::value, std::true_type, disjunction<Ts...>> {};

#endif

template<class T, class U>
struct copy_cv {
    using const_applied = conditional_t<std::is_const<T>::value, const U, U>;
    using type = conditional_t<std::is_volatile<T>::value, volatile const_applied, const_applied>;
};

template<class T, class U>
using copy_cv_t = typename copy_cv<T, U>::type;

template<class T, class U>
struct common_reference2 {
    using ref_remove_t = typename std::remove_reference<T>::type;
    using ref_remove_u = typename std::remove_reference<U>::type;
    using pure_t = typename std::remove_cv<ref_remove_t>::type;
    using pure_u = typename std::remove_cv<ref_remove_u>::type;

    using type = conditional_t<std::is_lvalue_reference<T>::value && std::is_lvalue_reference<U>::value &&
                                   std::is_same<pure_t, pure_u>::value,
                               copy_cv_t<ref_remove_t, ref_remove_u>&, typename std::common_type<pure_t, pure_u>::type>;
};

template<class T, class U>
struct common_reference2<T&, std::reference_wrapper<U>> {
    using type = std::reference_wrapper<copy_cv_t<T, U>>;
};

template<class T, class U>
struct common_reference2<std::reference_wrapper<T>, U&> {
    using type = std::reference_wrapper<copy_cv_t<T, U>>;
};

template<class T, class U>
using common_reference2_t = typename common_reference2<T, U>::type;

template<class... Ts>
struct common_reference;

template<class T>
struct common_reference<T> {
    using type = T;
};

template<class T1, class T2, class... Ts>
struct common_reference<T1, T2, Ts...> {
    template<class T>
    using clean_reference_wrapper = conditional_t<is_reference_wrapper<remove_cvref_t<T>>::value, remove_cvref_t<T>, T>;

    using type = typename common_reference<common_reference2_t<clean_reference_wrapper<T1>, clean_reference_wrapper<T2>>,
                                           clean_reference_wrapper<Ts>...>::type;
};

template<class Iterator, size_t N>
struct common_reference<homogeneous_array<Iterator, N>> {
    using type = ref_t<Iterator>;
};

template<class... Ts>
using common_reference_t = typename common_reference<Ts...>::type;

template<class>
struct iter_tuple_common_ref;

template<class... Iterators>
struct iter_tuple_common_ref<std::tuple<Iterators...>> {
    using type = common_reference_t<ref_t<Iterators>...>;
};

template<class Iterator, size_t I>
struct iter_tuple_common_ref<homogeneous_array<Iterator, I>> {
    using type = ref_t<Iterator>;
};

template<class IterTuple>
using iter_tuple_common_ref_t = typename iter_tuple_common_ref<IterTuple>::type;

using std::get;

template<class Iterable>
LZ_CONSTEXPR_CXX_17 iter_t<Iterable>
increment_or_decrement_iterator(Iterable&& iterable, const size_t this_size, const size_t min) {
    using diff_type = diff_iterable_t<Iterable>;
    // Check whether we can better use decrement or increment. For random access iterators, we can
    // use either, but for bidirectional iterators, we need to be "careful" about the direction.
    // We want to use the iterator that requires the least amount of increments/decrements.
    const auto to_decrement = static_cast<diff_type>(this_size - min);
    const auto to_increment = static_cast<diff_type>(min);
    if (to_increment > to_decrement) {
        return std::prev(detail::end(iterable), to_decrement);
    }
    return std::next(detail::begin(iterable), to_increment);
}

template<size_t... Is, class Iterable>
LZ_CONSTEXPR_CXX_17 auto
smallest_end_maybe_homo(Iterable&& iterable_tuple,
                        index_sequence<Is...>) -> maybe_homogeneous_t<decltype(detail::begin(get<Is>(iterable_tuple)))...> {
    const size_t sizes[] = { static_cast<size_t>(lz::eager_size(get<Is>(iterable_tuple)))... };
    const auto min = std::min({ sizes[Is]... });

    return { increment_or_decrement_iterator(get<Is>(std::forward<Iterable>(iterable_tuple)), sizes[Is], min)... };
}

template<class T, class IterableTuple, size_t... Is>
LZ_CONSTEXPR_CXX_14 maybe_homogeneous_t<decltype(Is, T{})...>
iterable_maybe_homo_eager_size_as(IterableTuple&& iterables, index_sequence<Is...>) {
    return { static_cast<T>(lz::eager_size(get<Is>(std::forward<IterableTuple>(iterables))))... };
}

template<class IterableTuple, size_t... I>
LZ_CONSTEXPR_CXX_14 auto
begin_maybe_homo_impl(IterableTuple&& iterable_tuple,
                      index_sequence<I...>) -> maybe_homogeneous_t<decltype(detail::begin(get<I>(iterable_tuple)))...> {
    return { detail::begin(get<I>(iterable_tuple))... };
}

template<class IterableTuple, size_t... I>
LZ_CONSTEXPR_CXX_14 auto
end_maybe_homo_impl(IterableTuple&& iterable_tuple,
                    index_sequence<I...>) -> maybe_homogeneous_t<decltype(detail::end(get<I>(iterable_tuple)))...> {
    return { detail::end(get<I>(iterable_tuple))... };
}

template<class IterableTuple>
LZ_CONSTEXPR_CXX_14 auto begin_maybe_homo(IterableTuple&& iterable_tuple)
    -> decltype(begin_maybe_homo_impl(std::forward<IterableTuple>(iterable_tuple),
                                      make_index_sequence<tuple_size<remove_cvref_t<IterableTuple>>::value>{})) {
    return begin_maybe_homo_impl(std::forward<IterableTuple>(iterable_tuple),
                                 make_index_sequence<tuple_size<remove_cvref_t<IterableTuple>>::value>{});
}

template<class IterableTuple>
LZ_CONSTEXPR_CXX_14 auto end_maybe_homo(IterableTuple&& iterable_tuple)
    -> decltype(end_maybe_homo_impl(std::forward<IterableTuple>(iterable_tuple),
                                    make_index_sequence<tuple_size<remove_cvref_t<IterableTuple>>::value>{})) {
    return end_maybe_homo_impl(std::forward<IterableTuple>(iterable_tuple),
                               make_index_sequence<tuple_size<remove_cvref_t<IterableTuple>>::value>{});
}
} // namespace detail
} // namespace lz

#endif
