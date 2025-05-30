#pragma once

#ifndef LZ_EXCLUDE_HPP
#define LZ_EXCLUDE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/exclude.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Excludes elements from a container, using two indexes. The first index is means the start index, the second index means
 * the end index. Contains a .size() method if the input iterable contains a .size() method, its iterator category is the same as
 * the input iterable. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire
 * sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`.
 * So, all in all: use lz::cache_size if:
 * - Your iterable is exactly bidirectional (so forward/random access excluded) and
 * - Your iterable is not sized and
 * - You either use multiple/a combination of the following iterables OR (see last point):
 * - `lz::chunks`
 * - `lz::enumerate`
 * - `lz::exclude`
 * - `lz::interleave`
 * - `lz::rotate`
 * - `lz::take`
 * - `lz::take_every`
 * - `lz::zip_longest`
 * - `lz::zip`
 * - Are planning to call begin() or end() multiple times on the same instance (with one or more of the above iterable
 * combinations) Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
 * // Exclude index [3, 6)
 * auto excluded = lz::exclude(vec, 3, 6); // excluded = { 1, 2, 3, 6, 7, 8, 9 }
 * // or
 * auto excluded = vec | lz::exclude(3, 6); // excluded = { 1, 2, 3, 6, 7, 8, 9 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::exclude_adaptor exclude{};

/**
 * @brief Helper alias for the exclude iterable.
 * @tparam Iterable The type of the iterable to exclude elements from.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
 * lz::exclude_iterable<std::vector<int>> excluded = lz::exclude(vec, 3, 6);
 * ```
 */
template<class Iterable>
using exclude_iterable = detail::exclude_iterable<Iterable>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_SKIP_HPP
