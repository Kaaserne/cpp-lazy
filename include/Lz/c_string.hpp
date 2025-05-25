#pragma once

#ifndef LZ_C_STRING_HPP
#define LZ_C_STRING_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/c_string.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to create a c forward iterable cstring iterable object. Its end() function will return a sentinel,
 * rather than an actual iterator. This iterable does not contain a .size() method. Example:
 * ```cpp
 * const char* str = "Hello, World!";
 * auto cstr = lz::c_string(str);
 * // or:
 * const char str[] = "Hello, World!";
 * auto cstr = str | lz::c_string;
 * ```
 */
constexpr detail::c_string_adaptor c_string{};

#else

/**
 * @brief This adaptor is used to create a c forward iterable cstring iterable object. Its end() function will return a sentinel,
 * rather than an actual iterator. This iterable does not contain a .size() method. Example:
 * ```cpp
 * const char* str = "Hello, World!";
 * auto cstr = lz::c_string(str);
 * // or:
 * const char str[] = "Hello, World!";
 * auto cstr = str | lz::c_string;
 * ```
 */
LZ_INLINE_VAR constexpr detail::c_string_adaptor c_string{};

#endif

/**
 * @brief Helper alias for the c_string_iterable.
 * @tparam CharT The character type of the string.
 * Example:
 * ```cpp
 * lz::c_string_iterable<char> cstr = lz::c_string("Hello, World!");
 * ```
*/
template<class CharT>
using c_string_iterable = detail::c_string_iterable<CharT>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif