#pragma once

#ifndef LZ_ROTATE_HPP
#define LZ_ROTATE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/rotate.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Rotates the input iterable by n elements. If `n` is greater than or equal to its size then the iterable will be empty.
 * Contains a .size() method if the input iterable also has a .size() method. Its iterator category is the same as the input
 * iterable. If the input iterable is forward or has a sentinel, it will return the end type of its input iterable, rather than a
 * rotate_iterator/default_sentinel_t. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto rotated = lz::rotate(vec, 2); // rotated = { 3, 4, 5, 1, 2 }
 * auto rotated = lz::rotate(vec, 50); // rotated = {}
 *
 * // or, in case input iterable is not forward:
 * auto str = lz::c_string("Hello, World!");
 * auto rotated = lz::rotate(str, 7); // rotated = "World!Hello, "
 * // rotated.end() will be a sentinel, rather than an actual iterator, as rotated str is forward
 * ```
 */
LZ_INLINE_VAR constexpr detail::rotate_adaptor rotate{};

/**
 * @brief Rotate iterable helper alias.
 * @tparam Iterable Type of the iterable to rotate.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * lz::rotate_iterable<std::vector<int>> rotated = lz::rotate(vec, 2);
 * ```
 */
template<class Iterable>
using rotate_iterable = detail::rotate_iterable<Iterable>;

} // namespace lz

#endif // LZ_ROTATE_HPP
