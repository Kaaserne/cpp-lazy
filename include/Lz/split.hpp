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
 * one of the following requirements: `ValueType(Iterator, Iterator)`, `ValueType(Iterator, std::size_t)`. The distance between
 * the iterators are calculated using std::distance, if the second constructor is used. Example:
 * ```cpp
 * std::string to_split = "H d";
 * auto splitter = to_split | lz::t_split<std::vector<char>>(" "); // { vector{'H'}, vector{'d'} }
 * // or
 * auto splitter = lz::t_split<std::vector<char>>(to_split, " "); // { vector{'H'}, vector{'d'} }
 * ```
 * @tparam ValueType The value type of the iterator to return, for example `std::vector<int>`.
 */
template<class ValueType>
static constexpr detail::split_adaptor<ValueType> detail::split_adaptor<ValueType>::t_split{};

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
 * ```
 */
static constexpr detail::split_adaptor<void> detail::split_adaptor<void>::split{};

/**
 * @brief Splits a string on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. Returns a std::string to its substrings. Example:
 * ```cpp
 * std::string str = "Hello, World!";
 * auto splitted = lz::s_split(str, ", "); // {"Hello", "World!"} where value_type is std::string
 * // auto splitted = lz::s_split(str, std::string(", ")); // Dangling reference, do not do this
 * // or
 * auto splitted = str | lz::s_split(", "); // {"Hello", "World!"} where value_type is std::string
 * // auto splitted = str | lz::s_split(std::string(", ")); // Dangling reference, do not do this
 * ```
 */
static constexpr detail::split_adaptor<std::basic_string> detail::split_adaptor<std::basic_string>::s_split{};

/**
 * @brief Splits a string on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. Returns a string_view to its substrings. Example:
 * ```cpp
 * std::string str = "Hello, World!";
 * auto splitted = lz::sv_split(str, ", "); // {"Hello", "World!"} where value_type is a string_view
 * // auto splitted = lz::sv_split(str, std::string(", ")); // Dangling reference, do not do this
 * // or
 * auto splitted = str | lz::s_split(", "); // {"Hello", "World!"} where value_type is a string_view
 * // auto splitted = str | lz::sv_split(std::string(", ")); // Dangling reference, do not do this
 * ```
 */
static constexpr detail::split_adaptor<lz::basic_string_view> detail::split_adaptor<lz::basic_string_view>::sv_split{};

#else

/**
 * @brief Splits an iterable on a delimiter. It returns a forward iterable, its end() method returns a default_sentinel and the
 * iterable does not contain a .size() method. The value type is equal to that of `ValueType`. `ValueType` constructor must meet
 * one of the following requirements: `ValueType(Iterator, Iterator)`, `ValueType(Iterator, std::size_t)`. The distance between
 * the iterators are calculated using std::distance, if the second constructor is used. Example:
 * ```cpp
 * std::string to_split = "H d";
 * auto splitter = to_split | lz::t_split<std::vector<char>>(" "); // { vector{'H'}, vector{'d'} }
 * // or
 * auto splitter = lz::t_split<std::vector<char>>(to_split, " "); // { vector{'H'}, vector{'d'} }
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
 * // auto splitted = str | lz::s_split(std::string(", ")); // Dangling reference, do not do this
 * // auto splitted = lz::s_split("Hello, World!", ", "); // Dangling reference, do not do this, instead do:
 * auto splitted = lz::s_split(lz::string_view("Hello, World!"), ", "); // {"Hello", "World!"}
 * // or
 * auto splitted = lz::s_split(lz::c_string("Hello, World!"), ", "); // slower alternative (not random access)
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
 * auto splitted = str | lz::sv_split(", "); // {"Hello", "World!"} where value_type is a string_view
 * // auto splitted = str | lz::sv_split(std::string(", ")); // Dangling reference, do not do this
 * // auto splitted = lz::sv_split("Hello, World!", ", "); // Dangling reference, do not do this, instead do:
 * auto splitted = lz::sv_split(lz::string_view("Hello, World!"), ", "); // {"Hello", "World!"}
 * // or
 * auto splitted = lz::sv_split(lz::c_string("Hello, World!"), ", "); // slower alternative (not random access)
 * ```
 */
LZ_INLINE_VAR constexpr detail::split_adaptor<lz::string_view> sv_split{};

#endif

using detail::split_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_STRING_SPLITTER_HPP