#pragma once

#ifndef LZ_REPEAT_HPP
#define LZ_REPEAT_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/repeat.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Returns an element n (or infinite) times. It returns a sentinel as its end() iterator. It contains a .size() method
 * which is equal to the amount of times the element is repeated (if it is not infinite). Its iterator category is random
 * access. Will return the same reference type as the reference type passed. Example:
 * ```cpp
 * auto repeater = lz::repeat(20, 5); // {20, 20, 20, 20, 20}
 * // infinite variant
 * auto repeater = lz::repeat(20); // {20, 20, 20, ...}
 * auto val = 3;
 * auto repeater =  lz::repeat(val, 5); // {3, 3, 3, 3, 3}, by reference
 * auto repeater =  lz::repeat(std::as_const(val)); // {3, 3, 3, ...}, by const reference
 * 
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
using repeat_iterable_inf = detail::repeat_iterable<T, true>;

/**
 * @brief Repeat finite helper alias type.
 * @tparam T The type of the element to repeat.
 * ```cpp
 * lz::repeat_iterable<int> iterable = lz::repeat(42, 3);
 * int val = 3;
 * lz::repeat_iterable_inf<int&> iterable = lz::repeat(val); // by reference
 * ```
 */
template<class T>
using repeat_iterable = detail::repeat_iterable<T, false>;

} // namespace lz

#endif
