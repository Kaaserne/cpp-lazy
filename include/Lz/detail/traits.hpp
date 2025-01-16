#pragma once

#ifndef LZ_LZ_TOOLS_HPP
#define LZ_LZ_TOOLS_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <array> // tuple_element
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace lz {
struct default_sentinel;

namespace detail {

template<class>
struct always_false : std::false_type {};

#ifdef LZ_HAS_CXX_11

#define MAKE_OPERATOR(OP, VALUE_TYPE) OP<VALUE_TYPE>

template<std::size_t, std::size_t...>
struct index_sequence_helper;

template<std::size_t N, std::size_t... Rest>
struct index_sequence_helper : public index_sequence_helper<N - 1, N - 1, Rest...> {};

template<std::size_t... Next>
struct index_sequence_helper<0, Next...> {
    using type = index_sequence_helper<Next...>;
};

template<std::size_t N>
using make_index_sequence = typename index_sequence_helper<N>::type;

template<class T>
using decay_t = typename std::decay<T>::type;

template<std::size_t I, class T>
using tup_element = typename std::tuple_element<I, T>::type;

#define MAKE_BIN_PRED(OP, VALUE_TYPE) OP<VALUE_TYPE>
#else // ^^^ has cxx 11 vvv cxx > 11
template<std::size_t... N>
using index_sequence_helper = std::index_sequence<N...>;

template<std::size_t N>
using make_index_sequence = std::make_index_sequence<N>;

template<class T>
using decay_t = std::decay_t<T>;

template<std::size_t I, class T>
using tup_element = std::tuple_element_t<I, T>;

#define MAKE_BIN_PRED(OP, VALUE_TYPE) OP<>

#endif // LZ_HAS_CXX_11

template<class Iterable>
LZ_NODISCARD constexpr auto begin(Iterable&& c) noexcept -> decltype(std::forward<Iterable>(c).begin());

template<class Iterable>
LZ_NODISCARD constexpr auto end(Iterable&& c) noexcept -> decltype(std::forward<Iterable>(c).end());

template<class T, size_t N>
LZ_NODISCARD constexpr auto begin(T (&array)[N]) noexcept -> decltype(std::begin(array));

template<class T, size_t N>
LZ_NODISCARD constexpr auto end(T (&array)[N]) noexcept -> decltype(std::end(array));

template<class T, size_t N>
LZ_NODISCARD constexpr auto begin(const T (&array)[N]) noexcept -> decltype(std::begin(array));

template<class T, size_t N>
LZ_NODISCARD constexpr auto end(const T (&array)[N]) noexcept -> decltype(std::end(array));

} // namespace detail

/**
 * @brief Can be used to get the iterator type of an iterable. Example: `lz::iter_t<std::vector<int>>` will return
 * `std::vector<int>::iterator`.
 *
 * @tparam Iterable The iterable to get the iterator type from.
 */
template<class Iterable>
using iter_t = decltype(detail::begin(std::forward<Iterable>(std::declval<Iterable>())));

/**
 * @brief Can be used to get the sentinel type of an iterable. Example: `lz::sentinel_t<std::vector<int>>` will return
 * `std::vector<int>::iterator`.
 * @tparam Iterable The iterable to get the sentinel type from.
 */
template<class Iterable>
using sentinel_t = decltype(detail::end(std::forward<Iterable>(std::declval<Iterable>())));

/**
 * @brief Can be used to get the value type of an iterator. Example: `lz::val_t<std::vector<int>::iterator>` will return `int`.
 *
 * @tparam Iterator The iterator to get the value type from.
 */
template<class Iterator>
using val_t = typename std::iterator_traits<typename std::remove_reference<Iterator>::type>::value_type;

/**
 * @brief Can be used to get the reference type of an iterator. Example: `lz::ref_t<std::vector<int>::iterator>` will return
 * `int&`.
 *
 * @tparam Iterator The iterator to get the reference type from.
 */
template<class Iterator>
using ref_t = typename std::iterator_traits<typename std::remove_reference<Iterator>::type>::reference;

/**
 * @brief Can be used to get the pointer type of an iterator. Example: `lz::ptr_t<std::vector<int>::iterator>` will return
 * `int*`.
 *
 * @tparam Iterator The iterator to get the pointer type from.
 */
template<class Iterator>
using ptr_t = typename std::iterator_traits<typename std::remove_reference<Iterator>::type>::pointer;

/**
 * @brief Can be used to get the difference type of an iterator. Example: `lz::diff_t<std::vector<int>::iterator>` will return
 * `std::ptrdiff_t`.
 *
 * @tparam Iterator The iterator to get the difference type from.
 */
template<class Iterator>
using diff_type = typename std::iterator_traits<typename std::remove_reference<Iterator>::type>::difference_type;

/**
 * @brief Can be used to get the iterator category of an iterator. Example: `lz::iter_cat_t<std::vector<int>::iterator>` will
 * return `std::random_access_iterator_tag`.
 *
 * @tparam Iterator The iterator to get the iterator category from.
 */
template<class Iterator>
using iter_cat_t = typename std::iterator_traits<typename std::remove_reference<Iterator>::type>::iterator_category;

/**
 * @brief Can be used to get the value type of an iterable. Example: `lz::val_iterable_t<std::vector<int>>` will return `int`.
 *
 * @tparam Iterable The iterable to get the value type from.
 */
template<class Iterable>
using val_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::value_type;

/**
 * @brief Can be used to get the difference type of an iterable. Example: `lz::diff_iterable_t<std::vector<int>>` will return
 * `std::ptrdiff_t`.
 *
 * @tparam Iterable
 */
template<class Iterable>
using diff_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::difference_type;

/**
 * @brief Can be used to get the reference type of an iterable. Example: `lz::ref_iterable_t<std::vector<int>>` will return
 * `int&`.
 *
 * @tparam Iterable The iterable to get the reference type from.
 */
template<class Iterable>
using ref_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::reference;

/**
 * @brief Can be used to get the pointer type of an iterable. Example: `lz::ptr_iterable_t<std::vector<int>>` will return `int*`.
 *
 * @tparam Iterable The iterable to get the pointer type from.
 */
template<class Iterable>
using ptr_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::pointer;

/**
 * @brief Can be used to get the iterator category of an iterable. Example: `lz::iter_cat_iterable_t<std::vector<int>>` will
 * return `std::random_access_iterator_tag`.
 *
 * @tparam Iterable The iterable to get the iterator category from.
 */
template<class Iterable>
using iter_cat_iterable_t = typename std::iterator_traits<iter_t<Iterable>>::iterator_category;

namespace detail {

template<class Function, class... Args>
using func_ret_type = decltype(std::declval<Function>()(std::declval<Args>()...));

template<class... Ts>
using common_type = typename std::common_type<Ts...>::type;

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

template<bool B>
struct enable_if_impl;

template<>
struct enable_if_impl<true> {
    template<class T>
    using type = T;
};

template<bool B, class T = void>
using enable_if = typename enable_if_impl<B>::template type<T>;

template<bool B>
struct conditional_impl;

template<>
struct conditional_impl<true> {
    template<class IfTrue, class /* IfFalse */>
    using type = IfTrue;
};

template<>
struct conditional_impl<false> {
    template<class /* IfTrue */, class IfFalse>
    using type = IfFalse;
};

template<bool B, class IfTrue, class IfFalse>
using conditional = typename conditional_impl<B>::template type<IfTrue, IfFalse>;

template<class T, class U, class... Vs>
struct is_all_same : std::integral_constant<bool, std::is_same<T, U>::value && is_all_same<U, Vs...>::value> {};

template<class T, class U>
struct is_all_same<T, U> : std::is_same<T, U> {};

template<class IterTag>
using is_bidi_tag = std::is_convertible<IterTag, std::bidirectional_iterator_tag>;

template<class Iterator>
using is_bidi = is_bidi_tag<iter_cat_t<Iterator>>;

template<class Iterator>
using is_fwd = std::is_convertible<iter_cat_t<Iterator>, std::forward_iterator_tag>;

template<class IterTag>
using is_ra_tag = std::is_convertible<IterTag, std::random_access_iterator_tag>;

template<class Iterator>
using is_ra = is_ra_tag<iter_cat_t<Iterator>>;

template<class Iterable>
using has_sentinel = std::is_same<iter_t<Iterable>, sentinel_t<Iterable>>;
} // namespace detail

/**
 * @brief Selects @p S if @p Tag is a `std::forward_iterator_tag`, otherwise selects @p Iterator.
 *
 * @tparam Tag The iterator tag to check.
 * @tparam Iterator The iterator type to select if @p Tag is not a `std::forward_iterator_tag`.
 * @tparam S The sentinel type to select if @p Tag is a `std::forward_iterator_tag`.
 */
template<class Tag, class Iterator, class S = default_sentinel>
using sentinel_selector = detail::conditional<std::is_same<Tag, std::forward_iterator_tag>::value, S, Iterator>;

/**
 * @brief Is @p TagFrom convertible to @p TagTo? If so, return @p TagFrom, otherwise return @p ToDecay.
 *
 * @tparam TagFrom The iterator tag to convert from.
 * @tparam TagTo The iterator tag to convert to.
 * @tparam ToDecay The type to decay to if @p TagFrom is not convertible to @p TagTo.
 */
template<class TagFrom, class TagTo, class ToDecay>
using iter_cat_decay = detail::conditional<std::is_convertible<TagFrom, TagTo>::value, TagFrom, ToDecay>;
} // namespace lz

#endif // LZ_LZ_TOOLS_HPP