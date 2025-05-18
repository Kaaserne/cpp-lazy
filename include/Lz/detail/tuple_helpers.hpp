#pragma once

#ifndef LZ_TUPLE_HELPERS_HPP
#define LZ_TUPLE_HELPERS_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/ref_or_view.hpp>
#include <Lz/detail/traits.hpp>
#include <tuple>

namespace lz {
namespace detail {

template<class T, std::size_t... Is>
std::tuple<decltype(Is, T{})...> tuple_of_n(index_sequence<Is...>) {
    return std::make_tuple(decltype(Is, T{})(0)...);
}

template<class>
struct iter_tuple_diff_type_helper;

template<class... Iterators>
struct iter_tuple_diff_type_helper<std::tuple<Iterators...>> {
    using type = common_type<diff_type<Iterators>...>;
};

template<class>
struct iter_tuple_iter_cat_helper;

template<class... Iterators>
struct iter_tuple_iter_cat_helper<std::tuple<Iterators...>> {
    using type = common_type<iter_cat_t<Iterators>...>;
};

template<class>
struct iter_tuple_value_type_helper;

template<class... Iterators>
struct iter_tuple_value_type_helper<std::tuple<Iterators...>> {
    using type = std::tuple<val_t<Iterators>...>;
};

template<class>
struct iter_tuple_ref_type_helper;

template<class... Iterators>
struct iter_tuple_ref_type_helper<std::tuple<Iterators...>> {
    using type = std::tuple<ref_t<Iterators>...>;
};

template<class IterTuple>
using iter_tuple_diff_type_t = typename iter_tuple_diff_type_helper<IterTuple>::type;

template<class IterTuple>
using iter_tuple_iter_cat_t = typename iter_tuple_iter_cat_helper<IterTuple>::type;

template<class IterTuple>
using iter_tuple_value_type_t = typename iter_tuple_value_type_helper<IterTuple>::type;

template<class IterTuple>
using iter_tuple_ref_type_t = typename iter_tuple_ref_type_helper<IterTuple>::type;

template<class Fn>
struct tuple_expand {
private:
    Fn _fn;

public:
    constexpr tuple_expand() = default;

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
smallest_end_tuple(Iterable&& iterable_tuple,
                   index_sequence<Is...>) -> std::tuple<decltype(std::begin(std::get<Is>(iterable_tuple)))...> {
    const std::size_t sizes[] = { static_cast<std::size_t>(lz::eager_size(std::get<Is>(iterable_tuple)))... };
    const auto min = std::min({ sizes[Is]... });

    return std::make_tuple(
        increment_or_decrement_iterator(std::get<Is>(std::forward<Iterable>(iterable_tuple)), sizes[Is], min)...);
}

template<class T, class IterableTuple, std::size_t... Is>
LZ_CONSTEXPR_CXX_14 std::tuple<decltype(Is, T{})...>
iterable_tuple_eager_size_as(IterableTuple&& iterables, index_sequence<Is...>) {
    return { static_cast<T>(lz::eager_size(std::get<Is>(std::forward<IterableTuple>(iterables))))... };
}

template<class Iterable, class... Ts>
LZ_CONSTEXPR_CXX_14 std::tuple<ref_or_view<remove_ref<Iterable>>, Ts...>
iterable_tuple_cat(Iterable&& iterable, const std::tuple<Ts...>& iterables) {
    return { std::tuple_cat(std::make_tuple(ref_or_view<remove_ref<Iterable>>(std::forward<Iterable>(iterable))), iterables) };
}

template<class Iterable, class... Ts>
LZ_CONSTEXPR_CXX_14 std::tuple<ref_or_view<remove_ref<Iterable>>, Ts...>
iterable_tuple_cat(Iterable&& iterable, std::tuple<Ts...>&& iterables) {
    return { std::tuple_cat(std::make_tuple(ref_or_view<remove_ref<Iterable>>(std::forward<Iterable>(iterable))),
                            std::move(iterables)) };
}

template<class IterableTuple, std::size_t... I>
LZ_CONSTEXPR_CXX_14 auto begin_tuple_impl(IterableTuple&& iterable_tuple, index_sequence<I...>)
    -> decltype(std::make_tuple(detail::begin(std::get<I>(std::forward<IterableTuple>(iterable_tuple)))...)) {
    return std::make_tuple(detail::begin(std::get<I>(std::forward<IterableTuple>(iterable_tuple)))...);
}

template<class IterableTuple, std::size_t... I>
LZ_CONSTEXPR_CXX_14 auto end_tuple_impl(IterableTuple&& iterable_tuple, index_sequence<I...>)
    -> decltype(std::make_tuple(detail::end(std::get<I>(std::forward<IterableTuple>(iterable_tuple)))...)) {
    return std::make_tuple(detail::end(std::get<I>(std::forward<IterableTuple>(iterable_tuple)))...);
}

template<class IterableTuple>
LZ_CONSTEXPR_CXX_14 auto begin_tuple(IterableTuple&& iterable_tuple)
    -> decltype(begin_tuple_impl(std::forward<IterableTuple>(iterable_tuple),
                                 make_index_sequence<std::tuple_size<decay_t<IterableTuple>>::value>{})) {
    return begin_tuple_impl(std::forward<IterableTuple>(iterable_tuple),
                            make_index_sequence<std::tuple_size<decay_t<IterableTuple>>::value>{});
}

template<class IterableTuple>
LZ_CONSTEXPR_CXX_14 auto end_tuple(IterableTuple&& iterable_tuple)
    -> decltype(end_tuple_impl(std::forward<IterableTuple>(iterable_tuple),
                               make_index_sequence<std::tuple_size<decay_t<IterableTuple>>::value>{})) {
    return end_tuple_impl(std::forward<IterableTuple>(iterable_tuple),
                          make_index_sequence<std::tuple_size<decay_t<IterableTuple>>::value>{});
}
} // namespace detail
} // namespace lz

#endif
