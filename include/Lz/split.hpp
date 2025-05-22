#pragma once

#ifndef LZ_STRING_SPLITTER_HPP
#define LZ_STRING_SPLITTER_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/split.hpp>
#include <Lz/string_view.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. The value type is equal to that of `ValueType`. `ValueType` constructor must meet
 * one of the following requirements: `ValueType(Iterator, Iterator)`, `ValueType(<decltype(std::adressof(Iterator))>,
 * std::size_t)`. The distance between the iterators are calculated using std::distance, if the second constructor is used.
 * Example:
 * ```cpp
 * std::string to_split = "H d";
 * auto splitter = to_split | lz::t_split<std::vector<char>>{}(" "); // { vector{'H'}, vector{'d'} }
 * // or
 * auto splitter = lz::t_split<std::vector<char>>{}(to_split, " "); // { vector{'H'}, vector{'d'} }
 *
 * // or
 * auto splitter = lz::t_split<std::vector<char>>{}(to_split, ' '); // { vector{'H'}, vector{'d'} }
 * // or
 * auto splitter = to_split | lz::t_split<std::vector<char>>{}(' '); // { vector{'H'}, vector{'d'} }
 * ```
 * @tparam ValueType The value type of the iterator to return, for example `std::vector<int>`.
 */
template<class ValueType>
using t_split = detail::split_adaptor<ValueType>;

/**
 * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. Example:
 * ```cpp
 * // Careful: split_values must be passed by reference, because it holds a reference to the values.
 * std::array<int, 4> arr = { 1, 2, 3, 4 };
 * auto split_values = {1, 2};
 * auto splitted = lz::split(arr, split_values); // {{1}, {4}}
 * // auto splitted = lz::split(arr, {1, 2}); // Dangling reference, do not do this
 * // or
 * auto splitted = arr | lz::split(split_valiues); // {{1}, {4}}
 * // auto splitted = arr | lz::split({1, 2}); // Dangling reference, do not do this
 *
 * // or
 * auto splitted = lz::split(arr, 2); // {{1}, {3, 4}}
 * // or
 * auto splitted = arr | lz::split(2); // {{1}, {3, 4}}
 * ```
 */
constexpr detail::split_adaptor<void> split{};

/**
 * @brief Splits a string on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. Returns a std::string to its substrings. Example:
 * ```cpp
 * std::string str = "Hello, World!";
 * auto splitted = lz::s_split(str, ", "); // {"Hello", "World!"} where value_type is std::string
 * // auto splitted = lz::s_split(str, std::string(", ")); // Dangling reference, do not do this
 * // or
 * auto splitted = str | lz::s_split(", "); // {"Hello", "World!"} where value_type is std::string
 * // auto splitted = str | lz::s_split(lz::string_view(", ")); // Dangling reference, do not do this
 *
 * // or
 * auto splitted = lz::s_split(str, ','); // {"Hello", " World!"} where value_type is std::string
 *
 * // or
 * auto splitted = str | lz::s_split(',') // {"Hello", " World!"} where value_type is std::string
 * ```
 */
constexpr detail::split_adaptor<std::string> s_split{};

/**
 * @brief Splits a string on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. Returns a string_view to its substrings. Example:
 * ```cpp
 * std::string str = "Hello, World!";
 * auto splitted = lz::sv_split(str, ", "); // {"Hello", "World!"} where value_type is a string_view
 * // auto splitted = lz::sv_split(str, std::string(", ")); // Dangling reference, do not do this
 * // or
 * auto splitted = str | lz::s_split(", "); // {"Hello", "World!"} where value_type is a string_view
 * // auto splitted = str | lz::sv_split(lz::string_view(", ")); // Dangling reference, do not do this
 *
 * // or
 * auto splitted = lz::sv_split(str, ','); // {"Hello", " World!"} where value_type is string_view
 *
 * // or
 * auto splitted = str | lz::sv_split(',') // {"Hello", " World!"} where value_type is string_view
 * ```
 */
constexpr detail::split_adaptor<lz::string_view> sv_split{};

#else

/**
 * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. The value type is equal to that of `ValueType`. `ValueType` constructor must meet
 * one of the following requirements: `ValueType(Iterator, Iterator)`, `ValueType(<decltype(std::adressof(Iterator))>,
 * std::size_t)`. The distance between the iterators are calculated using std::distance, if the second constructor is used.
 * Example:
 * ```cpp
 * std::string to_split = "H d";
 * auto splitter = to_split | lz::t_split<std::vector<char>>(" "); // { vector{'H'}, vector{'d'} }
 * // or
 * auto splitter = lz::t_split<std::vector<char>>(to_split, " "); // { vector{'H'}, vector{'d'} }
 *
 * // or
 * auto splitter = lz::t_split<std::vector<char>>(to_split, ' '); // { vector{'H'}, vector{'d'} }
 * // or
 * auto splitter = to_split | lz::t_split<std::vector<char>>(' '); // { vector{'H'}, vector{'d'} }
 * ```
 * @tparam ValueType The value type of the iterator to return, for example `std::vector<int>`.
 */
template<class ValueType>
LZ_INLINE_VAR constexpr detail::split_adaptor<ValueType> t_split{};

/**
 * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. Example:
 * ```cpp
 * // Careful: split_values must be passed by reference, because it holds a reference to the values.
 * std::array<int, 4> arr = { 1, 2, 3, 4 };
 * auto split_values = {1, 2};
 * auto splitted = lz::split(arr, split_values); // {{1}, {4}}
 * // auto splitted = lz::split(arr, {1, 2}); // Dangling reference, do not do this
 * // or
 * auto splitted = arr | lz::split(split_valiues); // {{1}, {4}}
 * // auto splitted = arr | lz::split({1, 2}); // Dangling reference, do not do this
 *
 * // or
 * auto splitted = lz::split(arr, 2); // {{1}, {3, 4}}
 * // or
 * auto splitted = arr | lz::split(2); // {{1}, {3, 4}}
 * ```
 */
LZ_INLINE_VAR constexpr detail::split_adaptor<void> split{};

/**
 * @brief Splits a string on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. Returns a std::string to its substrings. Example:
 * ```cpp
 * std::string str = "Hello, World!";
 * auto splitted = lz::s_split(str, ", "); // {"Hello", "World!"} where value_type is std::string
 * // auto splitted = lz::s_split(str, std::string(", ")); // Dangling reference, do not do this
 * // or
 * auto splitted = str | lz::s_split(", "); // {"Hello", "World!"} where value_type is std::string
 * // auto splitted = str | lz::s_split(lz::string_view(", ")); // Dangling reference, do not do this
 *
 * // or
 * auto splitted = lz::s_split(str, ','); // {"Hello", " World!"} where value_type is std::string
 *
 * // or
 * auto splitted = str | lz::s_split(',') // {"Hello", " World!"} where value_type is std::string
 * ```
 */
LZ_INLINE_VAR constexpr detail::split_adaptor<std::string> s_split{};

/**
 * @brief Splits a string on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. Returns a string_view to its substrings. Example:
 * ```cpp
 * std::string str = "Hello, World!";
 * auto splitted = lz::sv_split(str, ", "); // {"Hello", "World!"} where value_type is a string_view
 * // auto splitted = lz::sv_split(str, std::string(", ")); // Dangling reference, do not do this
 * // or
 * auto splitted = str | lz::s_split(", "); // {"Hello", "World!"} where value_type is a string_view
 * // auto splitted = str | lz::sv_split(lz::string_view(", ")); // Dangling reference, do not do this
 *
 * // or
 * auto splitted = lz::sv_split(str, ','); // {"Hello", " World!"} where value_type is string_view
 *
 * // or
 * auto splitted = str | lz::sv_split(',') // {"Hello", " World!"} where value_type is string_view
 * ```
 */
LZ_INLINE_VAR constexpr detail::split_adaptor<lz::string_view> sv_split{};

#endif

using detail::split_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_STRING_SPLITTER_HPP