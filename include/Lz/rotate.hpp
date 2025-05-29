#pragma once

#ifndef LZ_ROTATE_HPP
#define LZ_ROTATE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/rotate.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Rotates the input iterable by n elements. Contains a .size() method if the input iterable also has a .size() method.
 * Its iterator category is the same as the input iterable if it has a .size() method. If the input iterable does not have a
 * .size() method, it will return the end type of its input iterable, rather than a rotate_iterator. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto rotated = lz::rotate(vec, 2); // rotated = { 3, 4, 5, 1, 2 }
 *
 * // or
 * auto rotated = vec | lz::rotate(2); // rotated = { 3, 4, 5, 1, 2 }
 * // or, in case input iterable does not have .size() method:
 *
 * auto str = lz::c_string("Hello, World!");
 * auto rotated = lz::rotate(str, 7); // rotated = "World!Hello, "
 * // rotated.end() will be a sentinel, rather than an actual iterator, as rotated str does not have a .size() method
 *
 * // Invalid: filter decays into a forward iterator because it doesn't have a .size() method
 * // auto rotated =
 *         vec | lz::filter([](int i) { return i % 2 == 0; }) | lz::rotate(2) | lz::reverse; // Error, rotated is forward
 *
 * // Instead, do (first rotate, then filter, instead of filter then rotate):
 * auto rotated = vec | lz::rotate(2) | lz::filter([](int i) { return i % 2 == 0; }) | lz::reverse | lz::to<std::vector>();
 * // rotated = { 2, 4 }
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

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_ROTATE_HPP
