#pragma once

#ifndef LZ_REPEAT_HPP
#define LZ_REPEAT_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/repeat.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Returns an element n (or infinite) times. It returns a sentinel as its end() iterator. It contains a .size() method which is equal to
 * the amount of times the element is repeated (if it is not infinite). Its iterator category is forward. Example:
 * ```cpp
 * auto repeater = lz::repeat(20, 5); // {20, 20, 20, 20, 20}
 * // infinite variant
 * auto repeater = lz::repeat(20); // {20, 20, 20, ...}
 * ```
 */
constexpr detail::repeat_adaptor repeat{};

#else

/**
 * @brief Returns an element n (or infinite) times. It returns a sentinel as its end() iterator. It contains a .size() method which is equal to
 * the amount of times the element is repeated (if it is not infinite). Its iterator category is forward. Example:
 * ```cpp
 * auto repeater = lz::repeat(20, 5); // {20, 20, 20, 20, 20}
 * // infinite variant
 * auto repeater = lz::repeat(20); // {20, 20, 20, ...}
 * ```
 */
LZ_INLINE_VAR constexpr detail::repeat_adaptor repeat{};

#endif

using detail::repeat_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif