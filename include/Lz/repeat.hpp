#pragma once

#ifndef LZ_REPEAT_HPP
#define LZ_REPEAT_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/repeat.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

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

/**
 * @brief Repeat infinite helper alias type.
 * @tparam T The type of the element to repeat.
 * ```cpp
 * lz::repeat_iterable_inf<int> iterable = lz::repeat(42);
 * ```
 */
template<class T>
using repeat_iterable_inf = detail::repeat_iterable<true, T>;

/**
 * @brief Repeat finite helper alias type.
 * @tparam T The type of the element to repeat.
 * ```cpp
 * lz::repeat_iterable<int> iterable = lz::repeat(42, 3);
 * ```
 */
template<class T>
using repeat_iterable = detail::repeat_iterable<false, T>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif