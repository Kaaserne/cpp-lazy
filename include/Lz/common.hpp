#pragma once

#ifndef LZ_COMMON_HPP
#define LZ_COMMON_HPP

#include <Lz/detail/adaptors/common.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * Creates a common view from an iterator and a sentinel. The iterable must have a sentinel type (i.e. its begin() function
 * must return a different type than its end() function), otherwise a static assertion will fail. If the input iterable is
 * random access, it will return a basic_iterable. Otherwise, it will return a common_iterable. Example:
 * ```cpp
 * auto c_str = lz::c_string("Hello, World!"); // begin() and end() return different types
 * auto common_view = lz::common(c_str);
 * // or
 * auto common_view = lz::c_string("Hello, World!") | lz::common;
 * // now you can use common_view in <algorithm> functions
 *
 * // random access:
 * auto repeated = lz::repeat(20, 5); // begin() and end() return different types
 * auto common_view = lz::common(repeated); // common_view is a basic_iterable
 * // or
 * auto common_view = lz::repeat(20, 5) | lz::common;
 * ```
 * @attention Always try to use lz::common as late as possible. cpp-lazy is implemented in such a way that it will return
 * sentinels if possible to avoid duplicate data. So this for example:
 * ```cpp
 * auto common = lz::c_string("Hello, World!") | lz::common | lz::take(5);
 * ```
 * Will not result in an iterable where its end() and begin() functions return the same type. This is because
 * the common view is created first and then the take view is created. The take view will return another sentinel.
 */
LZ_INLINE_VAR constexpr detail::common_adaptor common{};

/**
 * @brief Helper type alias for the common iterable.
 * @tparam Iterable The iterable type.
 * ```cpp
 * lz::c_string_iterable<char> c_str = lz::c_string("Hello, World!"); // begin() and end() return different types
 * lz::common_iterable<lz::c_string_iterable<char>> common_view = lz::common(c_str);
 * ```
 */
template<class Iterable>
using common_iterable = decltype(lz::detail::common_adaptor{}(std::declval<Iterable>()));

} // namespace lz

#endif
