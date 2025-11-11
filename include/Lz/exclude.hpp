#pragma once

#ifndef LZ_EXCLUDE_HPP
#define LZ_EXCLUDE_HPP

#include <Lz/procs/chain.hpp>
#include <Lz/detail/adaptors/exclude.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Excludes elements from a container, using two indexes. The first index is means the start index, the second index means
 * the end index. Contains a .size() method if the input iterable contains a .size() method, its iterator category is the same as
 * the input iterable.
 * If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is traversed
 * to get its end size (using `lz::eager_size`); this can be inefficient. To prevent this traversal alltogether, you can use
 * `lz::iter_decay` defined in `<Lz/iter_tools.hpp>` or you can use `lz::cache_size` to cache the size of the iterable.
 * `lz::iter_decay` can decay the iterable into a forward one and since forward iterators cannot go backward, its entire size is
 * therefore also not needed to create an iterator from its end() function. `lz::cache_size` however will traverse the iterable
 * once and cache the size, so that subsequent calls to `end()` will not traverse the iterable again, but will return the cached
 * size instead. The following iterables require a(n) (eagerly)sized iterable:
 * - `lz::chunks`
 * - `lz::enumerate`
 * - `lz::exclude`
 * - `lz::interleave`
 * - `lz::rotate`
 * - `lz::take`
 * - `lz::take_every`
 * - `lz::zip_longest`
 * - `lz::zip`
 *
 * Example:
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

} // namespace lz

#endif // LZ_SKIP_HPP
