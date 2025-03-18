#pragma once

#ifndef LZ_COMMON_HPP
#define LZ_COMMON_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/common.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * Creates a common view from an iterator and a sentinel. The iterable must have a sentinel type (i.e. its begin() function
 * must return a different type than its end() function), otherwise a static assertion will fail.
 * Example:
 * ```cpp
 * auto c_str = lz::c_string("Hello, World!"); // begin() and end() return different types
 * auto common_view = lz::common(c_str);
 * // or
 * auto common_view = lz::c_string("Hello, World!") | lz::common;
 * // now you can use common_view in <algorithm> functions
 * ```
 * @attention Always try to use lz::common as late as possible. cpp-lazy is implemented in such a way that it will return
 * sentinels if possible to avoid duplicate data. So this for example:
 * ```cpp
 * auto common = lz::c_string("Hello, World!") | lz::common | lz::take(5);
 * ```
 * Will not result in an iterable where its end() and begin() functions return the same type. This is because
 * the common view is created first and then the take view is created. The take view will return another sentinel.
 */
static constexpr detail::common_adaptor detail::common_adaptor::common{};

#else

/**
 * Creates a common view from an iterator and a sentinel. The iterable must have a sentinel type (i.e. its begin() function
 * must return a different type than its end() function), otherwise a static assertion will fail.
 * Example:
 * ```cpp
 * auto c_str = lz::c_string("Hello, World!"); // begin() and end() return different types
 * auto common_view = lz::common(c_str);
 * // or
 * auto common_view = lz::c_string("Hello, World!") | lz::common;
 * // now you can use common_view in <algorithm> functions
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

#endif

using detail::common_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif
