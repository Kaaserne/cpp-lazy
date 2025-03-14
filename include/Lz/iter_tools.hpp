#pragma once

#ifndef LZ_ITER_TOOLS_HPP
#define LZ_ITER_TOOLS_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/iter_tools.hpp>
// #include <Lz/concatenate.hpp>
// #include <Lz/drop_while.hpp>
// #include <Lz/filter.hpp>
// #include <Lz/join.hpp>
// #include <Lz/take.hpp>
// #include <Lz/take_while.hpp>
#include <Lz/zip.hpp>
// #include <algorithm>
// #include <cctype>
// #include <numeric>

namespace lz {
namespace detail {
template<std::size_t I>
struct get_fn {
    template<class T>
    LZ_CONSTEXPR_CXX_14 auto operator()(T&& gettable) const noexcept -> decltype(std::get<I>(std::forward<T>(gettable))) {
        return std::get<I>(std::forward<T>(gettable));
    }
};

} // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Returns a split_iterable, that splits the string on `'\n'` using `lz::split`. Example:
 * ```cpp
 * std::string str = "Hello\nWorld\n!";
 * auto splitted = lz::lines(str); // {"Hello", "World", "!"}
 * // or
 * auto splitted = str | lz::lines; // {"Hello", "World", "!"}
 * ```
 */
static constexpr detail::lines_adaptor detail::lines_adaptor::lines{};

/**
 * @brief Returns an iterable that converts the elements in the given container to the type @p `T`, using `lz::map`.
 * @tparam T The type to convert the elements to.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto floats = lz::as<float>(vec); // { 1.f, 2.f, 3.f, 4.f, 5.f }
 * // or
 * auto floats = vec | lz::as<float>; // { 1.f, 2.f, 3.f, 4.f, 5.f }
 * ```
 */
template<class T>
static constexpr detail::as_adaptor<T> detail::as_adaptor<T>::as{};

/**
 * @brief Iterates over the elements in the given iterable and returns a tuple of `N` adjacent elements using `lz::zip`. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto iterable = lz::pairwise_n<2>(vec); // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * // or
 * auto iterable = vec | lz::pairwise_n<2>; // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * ```
 * @tparam N The amount of adjacent elements to return.
 */
template<std::size_t N>
LZ_INLINE_VAR constexpr detail::pairwise_n_adaptor<N> pairwise_n{};

/**
 * @brief Iterates over the elements in the given iterable and returns a tuple of 2 adjacent elements using `lz::zip`. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto iterable = lz::pairwise(vec); // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * // or
 * auto iterable = vec | lz::pairwise; // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * ```
 */
LZ_INLINE_VAR constexpr detail::pairwise_n_adaptor<2> pairwise{};

#else

/**
 * @brief Returns a split_iterable, that splits the string on `'\n'` using lz::split. Returns string_views to its substrings.
 * Example:
 * ```cpp
 * std::string str = "Hello\nWorld\n!";
 * auto splitted = lz::lines(str); // {"Hello", "World", "!"}
 * // or
 * auto splitted = str | lz::lines; // {"Hello", "World", "!"}
 * // or
 * auto splitted = lz::lines(lz::c_string("Hello\nWorld\n!")); // {"Hello", "World", "!"}
 * ```
 */
LZ_INLINE_VAR constexpr detail::lines_adaptor lines{};

/**
 * @brief Returns an iterable that converts the elements in the given container to the type @p `T` using `lz::map`.
 * @tparam T The type to convert the elements to.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto floats = lz::as<float>(vec); // { 1.f, 2.f, 3.f, 4.f, 5.f }
 * // or
 * auto floats = vec | lz::as<float>; // { 1.f, 2.f, 3.f, 4.f, 5.f }
 * ```
 */
template<class T>
LZ_INLINE_VAR constexpr detail::as_adaptor<T> as{};

/**
 * @brief Iterates over the elements in the given iterable and returns a tuple of `N` adjacent elements using `lz::zip`. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto iterable = lz::pairwise_n<2>(vec); // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * // or
 * auto iterable = vec | lz::pairwise_n<2>; // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * ```
 * @tparam N The amount of adjacent elements to return.
 */
template<std::size_t N>
LZ_INLINE_VAR constexpr detail::pairwise_n_adaptor<N> pairwise_n{};

/**
 * @brief Iterates over the elements in the given iterable and returns a tuple of 2 adjacent elements using `lz::zip`. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto iterable = lz::pairwise(vec); // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * // or
 * auto iterable = vec | lz::pairwise; // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * ```
 */
LZ_INLINE_VAR constexpr detail::pairwise_n_adaptor<2> pairwise{};

#endif

template<class Fn, class... Iterables>
using zip_with_iterable =
    decltype(lz::map(lz::zip(std::forward<Iterables>(std::declval<Iterables>())...), detail::make_expand_fn(std::declval<Fn>())));

/**
 * @brief Zips the given iterables together and applies the function @p `fn` on the elements using `lz::map` and `lz::zip`. Cannot
 * be used in a pipe expression. Example:
 * ```cpp
 * auto a = { 1, 2, 3 };
 * auto b = { 1, 2, 3 };
 * auto c = { 1, 2, 3 };
 * auto zipped = lz::zip_with([](int a, int b, int c) { return a + b + c; }, a, b, c); // {3, 6, 9}
 * ```
 *
 * @param iterables The iterables to zip together.
 * @param fn The function to apply on the elements.
 * @return A map object that can be iterated over.
 */
template<class Fn, class... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 zip_with_iterable<Fn, Iterables...> zip_with(Fn fn, Iterables&&... iterables) {
    return lz::map(lz::zip(std::forward<Iterables>(iterables)...), detail::make_expand_fn(std::move(fn)));
}

// /**
//  * @brief Returns an iterable object that returns all values from an iterable of which its `value_type` supports `std::get<1>`.
//  * Useful for example getting the values (instead of keys) from a `std::(unordered_)map`
//  * @param iterable The iterable to extract the elements from.
//  * @return An iterable object that returns the second value of `value_type` using `std::get<1>`. (works for any tuple-like
//  * interface)`
//  */
// template<LZ_CONCEPT_ITERABLE Iterable>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_20 map_iterable<iter_t<Iterable>, sentinel_t<Iterable>, detail::get_fn<1>>
// values(Iterable&& iterable) {
//     return lz::map(std::forward<Iterable>(iterable), detail::get_fn<1>());
// }

// /**
//  * @brief Returns an iterator object that returns all keys from an iterable of which its `value_type` supports `std::get<0>`.
//  * Useful for example getting the keys from a `std::(unordered_)map`
//  * @param iterable The iterable to extract the elements from.
//  * @return An iterable object that returns the first value of `value_type` using `std::get<0>`. (works for any tuple-like
//  * interface)`
//  */
// template<LZ_CONCEPT_ITERABLE Iterable>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_20 map_iterable<iter_t<Iterable>, sentinel_t<Iterable>, detail::get_fn<0>>
// keys(Iterable&& iterable) {
//     return lz::map(std::forward<Iterable>(iterable), detail::get_fn<0>());
// }

// template<class Iterable, class UnaryFilterPredicate, class UnaryMapOp>
// using filter_map_iterable =
//     map_iterable<detail::filter_iterator<iter_t<Iterable>, sentinel_t<Iterable>, UnaryFilterPredicate>,
//                  typename detail::filter_iterator<iter_t<Iterable>, sentinel_t<Iterable>, UnaryFilterPredicate>::sentinel,
//                  UnaryMapOp>;

// /**
//  * @brief Creates a map object with filter iterator that, if the filter function returns true, the map function is executed.
//  * @param iterable The iterable to filter/map.
//  * @param filter_predicate The function that filters the elements. If this function returns `true`, its corresponding container
//  * value is passed to the `map_op`.
//  * @param map_op The function that returns the (new) type.
//  * @return A map object that can be iterated over. The `value_type` of the this view object is equal to the return value of
//  * `map_op`.
//  */
// template<class Iterable, class UnaryFilterPredicate, class UnaryMapOp>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_20 filter_map_iterable<Iterable, UnaryFilterPredicate, UnaryMapOp>
// filter_map(Iterable&& iterable, UnaryFilterPredicate filter_predicate, UnaryMapOp map_op) {
//     return lz::map(lz::filter(std::forward<Iterable>(iterable), std::move(filter_predicate)), std::move(map_op));
// }

// // clang-format off
// template<class Iterable, class SelectorIterable>
// using select_iterable = filter_map_iterable<decltype(
//     lz::zip(std::forward<Iterable>(std::declval<Iterable>()),
//     std::forward<SelectorIterable>(std::declval<SelectorIterable>()))),
//                                    detail::get_fn<1>, detail::get_fn<0>>;
// // clang-format on

// /**
//  * @brief Selects elements from `iterable` where its corresponding index in `selectors` is `true` or convertible to `true`.
//  * @param iterable The iterable to select values from.
//  * @param selectors The selectors that specifies to select an item in `iterable` yes or no.
//  * @return A map object that can be iterated over with the excluded elements that `selectors` specify.
//  */
// template<class Iterable, class SelectorIterable>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_20 select_iterable<Iterable, SelectorIterable>
// select(Iterable&& iterable, SelectorIterable&& selectors) {
//     return filter_map(zip(std::forward<Iterable>(iterable), std::forward<SelectorIterable>(selectors)), detail::get_fn<1>(),
//                       detail::get_fn<0>());
// }

// template<class Iterable>
// using drop_back_while_iterable = detail::basic_iterable<std::reverse_iterator<std::reverse_iterator<iter_t<Iterable>>>>;

// /**
//  * @brief Trims the back of a sequence, as long as `predicate` returns true.
//  *
//  * @param iterable The iterable to trim
//  * @param predicate The unary predicate that trims the last elements while it returns `true`
//  * @return An iterable object.
//  */
// template<class Iterable, class UnaryPredicate>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_20 drop_back_while_iterable<Iterable>
// drop_back_while(Iterable&& iterable, UnaryPredicate predicate) {
//     static_assert(detail::is_bidi<iter_t<Iterable>>::value, "Iterator must be at least bidirectional");
//     return lz::reverse(lz::drop_while(lz::reverse(std::forward<Iterable>(iterable)), std::move(predicate)));
// }

// template<class Iterable>
// using trim_iterable = detail::basic_iterable<iter_t<drop_back_while_iterable<detail::basic_iterable<iter_t<Iterable>>>>>;

// /**
//  * Trims the beginning and ending of a sequence, as long as `first` returns true for the trimming of the
//  * beginning and as long as `last` returns true for the trimming of the ending.
//  * @param iterable The iterable to trim
//  * @param first The unary predicate that trims the first elements while it returns `true`
//  * @param last The unary predicate that trims the last elements while it returns `true`
//  * @return An iterator view object.
//  */
// template<class Iterable, class UnaryPredicateFirst, class UnaryPredicateLast>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_20 trim_iterable<Iterable>
// trim(Iterable&& iterable, UnaryPredicateFirst first, UnaryPredicateLast last) {
//     auto taken_first = drop_while(std::forward<Iterable>(iterable), std::move(first));
//     return drop_back_while(std::move(taken_first), std::move(last));
// }

// /**
//  * Trims a string and returns an iterator that skips whitespaces at the front and end.
//  * @param s The string to trim.
//  * @return The string, with trimmed spaces/tabs/newlines at the front and end.
//  */
// template<class String>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_20 trim_iterable<String> trim_string(String&& s) {
//     const auto is_space_fn = [](const char c) {
//         return static_cast<bool>(std::isspace(static_cast<unsigned char>(c)));
//     };
//     return lz::trim(std::forward<String>(s), is_space_fn, is_space_fn);
// }
} // End namespace lz

LZ_MODULE_EXPORT_SCOPE_END

#endif // LZ_ITER_TOOLS_HPP