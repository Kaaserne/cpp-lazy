#pragma once

#ifndef LZ_SLICE_HPP
#define LZ_SLICE_HPP

#include <Lz/detail/adaptors/slice.hpp>

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to slice an iterable from `from` to `to`. The iterator category is the same as the input
 * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. If its input
 * iterable has a .size() method, then this iterable will also have a .size() method.Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
 * auto slice = lz::slice(vec, 2, 6); // slice = { 3, 4, 5, 6 }
 * // or
 * auto slice = vec | lz::slice(2, 6); // slice = { 3, 4, 5, 6 }
 * ```
 */
static constexpr detail::slice_adaptor detail::slice_adaptor::slice{};

#else

/**
 * @brief This adaptor is used to slice an iterable from `from` to `to`. The iterator category is the same as the input
 * iterator category. Its end() function will return the same iterator as its input iterable. If its input iterable has a .size()
 * method, then this iterable will also have a .size() method.Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8 };
 * auto slice = lz::slice(vec, 2, 6); // slice = { 3, 4, 5, 6 }
 * // or
 * auto slice = vec | lz::slice(2, 6); // slice = { 3, 4, 5, 6 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::slice_adaptor slice{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_SLICE_HPP