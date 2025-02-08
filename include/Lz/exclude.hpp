#pragma once

#ifndef LZ_EXCLUDE_HPP
#define LZ_EXCLUDE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/exclude.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Excludes elements from a container, using two indexes. The first index is means the start index, the second index means
 * the end index. Contains a .size() method, it's a forward iterator and returns a sentinel. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
 * // Exclude index [3, 6)
 * auto excluded = lz::exclude(vec, 3, 6); // excluded = { 1, 2, 3, 6, 7, 8, 9 }
 * // or
 * auto excluded = vec | lz::exclude(3, 6); // excluded = { 1, 2, 3, 6, 7, 8, 9 }
 * ```
 */
static const detail::exclude_adaptor exclude{};

#else

/**
 * @brief Excludes elements from a container, using two indexes. The first index is means the start index, the second index means
 * the end index. Contains a .size() method, it's a forward iterator and returns a sentinel. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
 * // Exclude index [3, 6)
 * auto excluded = lz::exclude(vec, 3, 6); // excluded = { 1, 2, 3, 6, 7, 8, 9 }
 * // or
 * auto excluded = vec | lz::exclude(3, 6); // excluded = { 1, 2, 3, 6, 7, 8, 9 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::exclude_adaptor exclude{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_SKIP_HPP
