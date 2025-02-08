#pragma once

#ifndef LZ_REPEAT_HPP
#define LZ_REPEAT_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/repeat.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Returns an element n times. It returns a sentinel as its end() iterator. It contains a .size() method which is equal to
 * the amount of times the element is repeated. Its iterator category is forard. Example:
 * ```cpp
 * auto repeater = lz::repeat(20, 5); // {20, 20, 20, 20, 20}
 * // or
 * auto repeater = 20 | lz::repeat(5); // {20, 20, 20, 20, 20}
 * ```
 */
static const detail::repeat_adaptor repeat{};

#else

/**
 * @brief Returns an element n times. It returns a sentinel as its end() iterator. It contains a .size() method which is equal to
 * the amount of times the element is repeated. Its iterator category is forard. Example:
 * ```cpp
 * auto repeater = lz::repeat(20, 5); // {20, 20, 20, 20, 20}
 * // or
 * auto repeater = 20 | lz::repeat(5); // {20, 20, 20, 20, 20}
 * ```
 */
LZ_INLINE_VAR constexpr detail::repeat_adaptor repeat{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif