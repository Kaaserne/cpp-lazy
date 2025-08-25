#pragma once

#ifndef LZ_SLICE_HPP
#define LZ_SLICE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/slice.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief This adaptor is used to slice an iterable from `from` to `to`. The iterator category is the same as the input
 * iterator category. Its end() function will return the same iterator as its input iterable or a default_sentinel_t if the input
 * iterable is forward. If its input iterable has a .size() method, then this iterable will also have a .size() method.Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
 * auto slice = lz::slice(vec, 2, 6); // slice = { 3, 4, 5, 6 }
 * // or
 * auto slice = vec | lz::slice(2, 6); // slice = { 3, 4, 5, 6 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::slice_adaptor slice{};

/**
 * @brief Slice iterable helper alias.
 * @tparam Iterable The type of the iterable to slice.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
 * lz::slice_iterable<std::vector<int>> slice_vec = lz::slice(vec, 2, 6);
 * ```
 */
template<class Iterable>
using slice_iterable = detail::slice_iterable<Iterable>;

} // namespace lz

#endif // LZ_SLICE_HPP
