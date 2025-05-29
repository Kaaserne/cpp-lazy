#pragma once

#ifndef LZ_ITER_TOOLS_HPP
#define LZ_ITER_TOOLS_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/iter_tools.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Zip with iterable helper alias
 * @tparam Fn The function to apply on the elements of the zipped iterables.
 * @tparam Iterables The iterables to zip together.
 * ```cpp
 * std::vector<int> a = { 1, 2, 3 };
 * std::vector<int> b = { 1, 2, 3 };
 * using zip_t = lz::zip_with_iterable<std::function<int(int, int)>, std::vector<int>, std::vector<int>>;
 * zip_t zipped = lz::zip_with([](int a, int b) { return a + b; }, a, b);
 */
template<class Fn, class... Iterables>
using zip_with_iterable =
    decltype(lz::map(lz::zip(std::forward<Iterables>(std::declval<Iterables>())...), detail::make_expand_fn(std::declval<Fn>())));

/**
 * @brief Lines iterable helper alias
 *
 * @tparam Iterable The iterable to split into lines.
 * @tparam CharT The character type of the string to split into lines. Defaults to `char`.
 * ```cpp
 * std::string str = "Hello\nWorld\n!";
 * lz::lines_iterable<std::string> splitted = lz::lines(str);
 * ```
 */
template<class Iterable, class CharT = char>
using lines_iterable = detail::lines_iterable<CharT, Iterable>;

/**
 * @brief Lines iterable helper alias for string_views. string_views are not held by reference, but copied.
 *
 * @tparam Iterable The iterable to split into lines.
 * @tparam CharT The character type of the string to split into lines. Defaults to `char`.
 * ```cpp
 * lz::lines_iterable_sv<> actual = lz::lines(lz::string_view("hello world\nthis is a message\ntesting"));
 * ```
 */
template<class CharT = char>
using lines_iterable_sv = lines_iterable<lz::copied_iterable<lz::basic_string_view<CharT>>, CharT>;

/**
 * @brief As iterable helper alias
 *
 * @tparam Iterable The iterable to convert the elements of.
 * @tparam T The type to convert the elements to.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * // c++ 11
 * lz::as_iterable<std::vector<int>, float> iterable = lz::as<float>{}(vec);
 * // c++ 14 and later
 * lz::as_iterable<std::vector<int>, float> iterable = lz::as<float>(vec);
 */
template<class Iterable, class T>
using as_iterable = detail::as_iterable<Iterable, T>;

/**
 * @brief Pairwise iterable helper alias
 *
 * @tparam Iterable The iterable to iterate over.
 * @tparam N The number of adjacent elements to return in a tuple.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * // c++ 11
 * lz::pairwise_n_iterable<std::vector<int>, 2> iterable = lz::pairwise_n<2>{}(vec);
 * // c++ 14 and later
 * lz::pairwise_n_iterable<std::vector<int>, 2> iterable = lz::pairwise_n<2>(vec);
 * ```
 */
template<class Iterable, std::size_t N>
using pairwise_n_iterable = detail::pairwise_n_iterable<Iterable, N>;

/**
 * @brief Pairwise iterable helper alias
 *
 * @tparam Iterable The iterable to iterate over.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * lz::pairwise_iterable<std::vector<int>> iterable = lz::pairwise(vec);
 * ```
 */
template<class Iterable, std::size_t N>
using pairwise_iterable = pairwise_n_iterable<Iterable, 2>;

/**
 * @brief Keys, values and get_nth iterable helper alias
 *
 * @tparam Iterable The iterable to get the nth element from.
 * @tparam N The index of the element to get from the iterable. 0 for keys, 1 for values, and any other index for the nth element.
 * ```cpp
 * std::vector<std::tuple<int, int, int>> three_tuple_vec = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
 * lz::get_nth_iterable<std::vector<std::tuple<int, int, int>>, 2> iterable = lz::get_nth<2>(three_tuple_vec);
 * lz::get_nth_iterable<std::vector<std::tuple<int, int, int>>, 0> keys_iterable = lz::keys(three_tuple_vec);
 * ```
 */
template<class Iterable, std::size_t N>
using get_nth_iterable = detail::get_nth_iterable<Iterable, N>;

/**
 * @brief Filter map iterable helper alias
 *
 * @tparam Iterable The iterable to filter and map.
 * @tparam UnaryFilterPredicate The predicate to filter the elements with.
 * @tparam UnaryMapOp The function to map the filtered elements with.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * using filter_map_iterable = lz::filter_map_iterable<std::vector<int>, std::function<bool(int)>, std::function<int(int)>>;
 * filter_map_iterable fm_iterable = lz::filter_map(vec, [](int i) { return i % 2 == 0; }, [](int i) { return i * 2; });
 * ```
 */
template<class Iterable, class UnaryFilterPredicate, class UnaryMapOp>
using filter_map_iterable = detail::filter_map_iterable<Iterable, UnaryFilterPredicate, UnaryMapOp>;

/**
 * @brief Select iterable helper alias
 * @tparam Iterable The iterable to select elements from.
 * @tparam SelectorIterable The iterable that contains the selection criteria (true/false).
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::vector<bool> selector = { true, false, true, false, true };
 * using select_iterable = lz::select_iterable<std::vector<int>, std::vector<bool>>;
 * select_iterable selected = lz::select(vec, selector);
 * ```
 */
template<class Iterable, class SelectorIterable>
using select_iterable = detail::select_iterable<Iterable, SelectorIterable>;

/**
 * @brief Drop back iterable helper alias
 * @tparam Iterable The iterable to drop elements from the back.
 * @tparam UnaryPredicate The predicate to determine which elements to drop from the back.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * using drop_back_iterable = lz::drop_back_iterable<std::vector<int>, std::function<bool(int)>>;
 * drop_back_iterable dropped = lz::drop_back_while(vec, [](int i) { return i > 3; });
 * ```
 */
template<class Iterable, class UnaryPredicate>
using drop_back_iterable = detail::drop_back_iterable<Iterable, UnaryPredicate>;

/**
 * @brief Trim iterable helper alias
 * @tparam Iterable The iterable to trim.
 * @tparam UnaryPredicateFirst The predicate to determine which elements to drop from the front.
 * @tparam UnaryPredicateLast The predicate to determine which elements to drop from the back.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * using trim_iterable = lz::trim_iterable<std::vector<int>, std::function<bool(int)>, std::function<bool(int)>>;
 * trim_iterable trimmed = lz::trim(vec, [](int i) { return i < 3; }, [](int i) { return i > 4; });
 * ```
 */
template<class Iterable, class UnaryPredicateFirst, class UnaryPredicateLast>
using trim_iterable = detail::trim_iterable<Iterable, UnaryPredicateFirst, UnaryPredicateLast>;

/**
 * @brief Trim string helper alias
 * @tparam String The string type to trim, for example `std::string` or `std::wstring`.
 * ```cpp
 * std::string str = "  hello  ";
 * lz::trim_string_iterable<std::string> trimmed = lz::trim(str); // "hello"
 * ```
 */
template<class String>
using trim_string_iterable = trim_iterable<String, detail::trim_fn, detail::trim_fn>;

/**
 * @brief Zips the given iterables together and applies the function @p `fn` on the elements using `lz::map` and `lz::zip`. Cannot
 * be used in a pipe expression. Example:
 * ```cpp
 * std::vector<int> a = { 1, 2, 3 };
 * std::vector<int> b = { 1, 2, 3 };
 * std::vector<int> c = { 1, 2, 3 };
 * auto zipped = lz::zip_with([](int a, int b, int c) { return a + b + c; }, a, b, c); // {3, 6, 9}
 * ```
 *
 * @param iterables The iterables to zip together.
 * @param fn The function to apply on the elements.
 * @return A map object that can be iterated over.
 */
template<class Fn, class... Iterables>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 zip_with_iterable<Fn, detail::remove_ref<Iterables>...>
zip_with(Fn fn, Iterables&&... iterables) {
    return lz::map(lz::zip(std::forward<Iterables>(iterables)...), detail::make_expand_fn(std::move(fn)));
}

// TODO unzip_with (and unzip?)

#ifdef LZ_HAS_CXX_11

/**
 * @brief Returns an iterable that converts the elements in the given container to the type @p `T`, using `lz::map` and
 * `static_cast`.
 * @tparam T The type to convert the elements to.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto floats = lz::as<float>{}(vec); // { 1.f, 2.f, 3.f, 4.f, 5.f }
 * // or
 * auto floats = vec | lz::as<float>{}; // { 1.f, 2.f, 3.f, 4.f, 5.f }
 * ```
 */
template<class T>
using as = detail::as_adaptor<T>;

/**
 * @brief Iterates over the elements in the given iterable and returns a tuple of `N` adjacent elements using `lz::zip` and
 * `lz::drop`. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto iterable = lz::pairwise_n<2>{}(vec); // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * // or
 * auto iterable = vec | lz::pairwise_n<2>{}; // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * ```
 * @tparam N The amount of adjacent elements to return.
 */
template<std::size_t N>
using pairwise_n = detail::pairwise_n_adaptor<N>;

/**
 * @brief Gets the nth element from a std::get-able container, using `std::get` and `lz::map`. Example:
 * ```cpp
 * std::vector<std::tuple<int, int, int>> three_tuple_vec = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
 * auto actual = lz::get_nth<2>{}(three_tuple_vec); // {3, 6, 9}
 * // or
 * auto actual = three_tuple_vec | lz::get_nth<2>{}; // {3, 6, 9}
 * ```
 * @tparam I The index to get from the std::get-able container.
 */
template<std::size_t I>
using get_nth = detail::get_n_adaptor<I>;

#else

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
 * @brief Iterates over the elements in the given iterable and returns a tuple of `N` adjacent elements using `lz::zip` and
 * `lz::drop`. Example:
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
 * @brief Gets the nth element from a std::get-able container, using `std::get` and `lz::map`. Example:
 * ```cpp
 * std::vector<std::tuple<int, int, int>> three_tuple_vec = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
 * auto actual = lz::get_nth<2>(three_tuple_vec); // {3, 6, 9}
 * // or
 * auto actual = three_tuple_vec | lz::get_nth<2>; // {3, 6, 9}
 * ```
 * @tparam I The index to get from the std::get-able container.
 */
template<std::size_t I>
LZ_INLINE_VAR constexpr detail::get_n_adaptor<I> get_nth{};

#endif

/**
 * @brief Returns a split_iterable, that splits the string on `'\n'` using lz::split. Returns string_views to its substrings.
 * Example:
 * ```cpp
 * std::string str = "Hello\nWorld\n!";
 * auto splitted = lz::lines(str); // {"Hello", "World", "!"}
 * lz::string_view str_view = "Hello\nWorld\n!";
 *
 * // splitted_view, does not hold a reference to the original string
 * auto splitted_view = lz::lines(str_view); // {"Hello", "World", "!"},
 * // or
 * auto splitted = str | lz::lines; // {"Hello", "World", "!"}
 * // or
 * auto splitted = lz::lines(lz::c_string("Hello\nWorld\n!")); // {"Hello", "World", "!"}
 * ```
 */
LZ_INLINE_VAR constexpr detail::lines_adaptor lines{};

/**
 * @brief Iterates over the elements in the given iterable and returns a tuple of 2 adjacent elements using `lz::zip` and
 * `lz::drop`. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto iterable = lz::pairwise(vec); // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * // or
 * auto iterable = vec | lz::pairwise; // { {1, 2}, {2, 3}, {3, 4}, {4, 5} }
 * ```
 */
LZ_INLINE_VAR constexpr detail::pairwise_n_adaptor<2> pairwise{};

/**
 * @brief Gets the keys from a std::get-able container, using `std::get<0>` and `lz::map`. Example:
 * ```cpp
 * std::map<int, std::string> m = { { 1, "hello" }, { 2, "world" }, { 3, "!" } };
 * auto keys = lz::keys(m); // {1, 2, 3}
 * // or
 * auto keys = m | lz::keys; // {1, 2, 3}
 * ```
 */
LZ_INLINE_VAR constexpr detail::get_n_adaptor<0> keys{};

/**
 * @brief Gets the values from a std::get-able container, using `std::get<1>` and `lz::map`. Example:
 * ```cpp
 * std::map<int, std::string> m = { { 1, "hello" }, { 2, "world" }, { 3, "!" } };
 * auto values = lz::values(m); // {"hello", "world", "!"}
 * // or
 * auto values = m | lz::values; // {"hello", "world", "!"}
 * ```
 */
LZ_INLINE_VAR constexpr detail::get_n_adaptor<1> values{};

/**
 * @brief Filters an iterable using `predicate` and then maps those elements using `fn`, using `lz::filter` and `lz::map`.
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto fm = lz::filter_map(vec, [](int i) { return i % 2 == 0; }, [](int i) { return i * 2; }); // {4, 8}
 * // or
 * auto fm = vec | lz::filter_map([](int i) { return i % 2 == 0; }, [](int i) { return i * 2; }); // {4, 8}
 * ```
 */
LZ_INLINE_VAR constexpr detail::filter_map_adaptor filter_map{};

/**
 * @brief Selects elements from the first iterable if the corresponding element in the second iterable is `true`, using
 * `lz::filter`, `lz::zip` and `lz::map`. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * std::vector<bool> bools = { true, false, true, false, true };
 * auto selected = lz::select(vec, bools); // {1, 3, 5}
 * // or
 * auto selected = vec | lz::select(bools); // {1, 3, 5}
 * ```
 */
LZ_INLINE_VAR constexpr detail::select_adaptor select{};

/**
 * @brief Trims the back of the sequence as long as `predicate` returns `true`, then reverses the sequence again. Essentially
 * turning the predicate into a not fn, using `lz::reverse`, `lz::drop_back_while` followed by another `lz::reverse`. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto trimmed = lz::drop_back_while(vec, [](int i) { return i > 3; }); // {1, 2, 3}
 * // or
 * auto trimmed = vec | lz::drop_back_while([](int i) { return i > 3; }); // {1, 2, 3}
 * ```
 */
LZ_INLINE_VAR constexpr detail::drop_back_while_adaptor drop_back_while{};

/**
 * Trims the beginning and ending of a sequence, as long as the first predicate returns true for the trimming of the
 * beginning and as long as the last predicate returns true for the trimming of the ending. This is done using `lz::drop_while`
 * and `lz::drop_back_while`. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto trimmed = lz::trim(vec, [](int i) { return i < 3; }, [](int i) { return i > 4; }); // {3}
 * // or
 * auto trimmed = vec | lz::trim([](int i) { return i < 3; }, [](int i) { return i > 4; }); // {3}
 * std::string str = "  hello  ";
 * auto trimmed = lz::trim(str); // "hello", uses std::isspace
 * // or
 * auto trimmed = str | lz::trim; // "hello", uses std::isspace
 * ```
 */
LZ_INLINE_VAR constexpr detail::trim_adaptor trim{};

LZ_MODULE_EXPORT_SCOPE_END

} // End namespace lz

#endif // LZ_ITER_TOOLS_HPP