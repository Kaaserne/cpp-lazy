#pragma once

#ifndef LZ_ITER_TOOLS_HPP
#define LZ_ITER_TOOLS_HPP

#include <Lz/basic_iterable.hpp>
// #include <Lz/concatenate.hpp>
// #include <Lz/drop_while.hpp>
// #include <Lz/filter.hpp>
// #include <Lz/join.hpp>
// #include <Lz/map.hpp>
// #include <Lz/split.hpp>
// #include <Lz/take.hpp>
// #include <Lz/take_while.hpp>
// #include <Lz/zip.hpp>
// #include <algorithm>
// #include <cctype>
// #include <numeric>

namespace lz {
// namespace detail {
// template<class To>
// struct convert_fn {
//     template<class From>
//     constexpr To operator()(From&& f) const {
//         return static_cast<To>(f);
//     }

//     template<class From>
//     constexpr To operator()(From&& f) {
//         return static_cast<To>(f);
//     }
// };

// template<std::size_t I>
// struct get_fn {
//     // TODO add const variant

//     template<class T>
//     LZ_CONSTEXPR_CXX_14 auto operator()(T&& gettable) noexcept -> decltype(std::get<I>(std::forward<T>(gettable))) {
//         return std::get<I>(std::forward<T>(gettable));
//     }
// };

// template<class Iterator, std::size_t... N>
// using iter_tuple = decltype(std::make_tuple(std::next(std::declval<Iterator>(), N)...));

// template<class Iterator, std::size_t... N>
// using sent_tuple = decltype(std::make_tuple((N, std::declval<Iterator>())...));

// template<class Iterable, std::size_t... N>
// using pairwise_n_object = zip_iterable<decltype(std::make_tuple(std::next(std::begin(std::declval<Iterable>()), N)...)),
//                                        decltype(std::make_tuple(std::end((N, std::declval<Iterable>()))...))>;

// template<std::size_t N, class Iterator>
// LZ_CONSTEXPR_CXX_14 enable_if<N == 0, decay_t<Iterator>> cached_next(Iterator&& iterator) {
//     return std::forward<Iterator>(iterator);
// }

// template<std::size_t N, class Iterator>
// LZ_CONSTEXPR_CXX_14 enable_if<N != 0, decay_t<Iterator>> cached_next(Iterator&& iterator) {
//     return std::next(cached_next<N - 1>(std::forward<Iterator>(iterator)), 1);
// }

// template<class Iterable, std::size_t... N>
// LZ_CONSTEXPR_CXX_14 pairwise_n_object<Iterable, N...> pairwise_n_construct(Iterable&& iterable, index_sequence_helper<N...>) {
//     return { std::make_tuple(cached_next<N>(std::begin(iterable))...), std::make_tuple(std::end((N, iterable))...) };
// }
// } // namespace detail

LZ_MODULE_EXPORT_SCOPE_BEGIN

// /**
//  * @brief Returns a split_iterable, that splits the string on `'\n'`.
//  * @param string The string to split on.
//  * @return Returns a split_iterable iterator, that splits the string on `'\n'`.
//  */
// template<class String>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_20 string_splitter<String, typename String::value_type> lines(const String& string) {
//     return lz::split(string, "\n");
// }

// /**
//  * @brief The exact opposite of `lines`. It joins a container of `std::string` or `std::string_view` container with `'\n'` as
//  * delimiter.
//  * @param strings The container of a string type
//  * @return A Join iterator that joins the strings in the container on `'\n'`.
//  */
// template<class Strings>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_20 join_iterable<iter_t<Strings>, sentinel_t<Strings>> unlines(Strings&& strings) {
//     return lz::join(strings, "\n");
// }

/**
 * @brief Returns an iterable object of which its iterators are reversed.
 * @param iterable The iterable. The iterable must have at least std::bidirectional_iterator_tag.
 * @return An iterable object that contains the reverse order of [begin(iterable) end(iterable))
 */
template<LZ_CONCEPT_BIDIRECTIONAL_ITERABLE Iterable>
LZ_NODISCARD LZ_CONSTEXPR_CXX_17 basic_iterable<std::reverse_iterator<iter_t<Iterable>>> reverse(Iterable&& iterable) {
    static_assert(detail::is_bidi<iter_t<Iterable>>::value, "Iterable must be at least a bidirectional iterable");
    using iterator = iter_t<Iterable>;
    std::reverse_iterator<iterator> rev_begin(detail::begin(std::forward<Iterable>(iterable)));
    std::reverse_iterator<iterator> rev_end(detail::end(std::forward<Iterable>(iterable)));
    return { std::move(rev_end), std::move(rev_begin) };
}

// /**
//  * @brief Returns an iterable that converts the elements in the given container to the type @p `T`.
//  * @tparam T The type to convert the elements to.
//  * @return A map iterator that converts the elements in the container to the type @p `T`.
//  */
// template<class T, LZ_CONCEPT_ITERATOR Iterable>
// LZ_NODISCARD constexpr map_iterable<iter_t<Iterable>, sentinel_t<Iterable>, detail::convert_fn<T>> as(Iterable&& iterable) {
//     return lz::map(std::forward<Iterable>(iterable), detail::convert_fn<T>());
// }

// template<class Fn, class... Iterables>
// using zip_with_iterable = map_iterable<decltype(lz::zip(std::forward<Iterables>(std::declval<Iterables>())...).begin()),
//                                        decltype(lz::zip(std::forward<Iterables>(std::declval<Iterables>())...).end()),
//                                        decltype(detail::make_expand_fn(std::move(std::declval<Fn>()),
//                                                                        detail::make_index_sequence<sizeof...(Iterables)>()))>;

// /**
//  * @brief Zips n containers and applies (simultaneously) the function given as argument, containing each of the containers'
//  value
//  * types. Signature must look like: `fn(Iter1::value_type, Iter2::value_type, ..., IterN::value_type)`.
//  * @param fn The function to apply to each elements in all containers.
//  * @param iterables The iterables.
//  * @return A map_iterable<zip_iterable> object that applies fn over each expanded tuple elements from [begin, end)
//  */
// template<class Fn, class... Iterables>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_14 zip_with_iterable<Fn, Iterables...> zip_with(Fn fn, Iterables&&... iterables) {
//     auto zipper = lz::zip(std::forward<Iterables>(iterables)...);
//     auto tuple_expander_func = detail::make_expand_fn(std::move(fn), detail::make_index_sequence<sizeof...(Iterables)>());
//     return lz::map(std::move(zipper), std::move(tuple_expander_func));
// }

// template<class Iterable, std::size_t N>
// using pairwise_iterable =
//     decltype(detail::pairwise_n_construct(std::forward<Iterable>(std::declval<Iterable>()), detail::make_index_sequence<N>()));

// /**
//  * @brief Returns an iterable that accesses N adjacent elements of one container in a std::tuple<T, ...> like fashion.
//  *
//  * @tparam N The amount of elements to access.
//  * @param iterable The container/iterable.
//  * @return A zip iterator that accesses N adjacent elements of one container.
//  */
// template<std::size_t N, LZ_CONCEPT_ITERABLE Iterable>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_14 pairwise_iterable<Iterable, N> pairwise_n(Iterable&& iterable) {
//     return detail::pairwise_n_construct(std::forward<Iterable>(iterable), detail::make_index_sequence<N>());
// }

// /**
//  * @brief Returns an iterable that accesses two adjacent elements of one container in a std::tuple<T, T> like fashion.
//  * @param iterable A container/iterable.
//  * @return A zip iterator that accesses two adjacent elements of one container.
//  */
// template<LZ_CONCEPT_ITERABLE Iterable>
// LZ_NODISCARD LZ_CONSTEXPR_CXX_14 pairwise_iterable<Iterable, 2> pairwise(Iterable&& iterable) {
//     LZ_ASSERT(lz::has_many(iterable), "length of the sequence must be greater than or equal to 2");
//     return pairwise_n<2>(std::forward<Iterable>(iterable));
// }

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