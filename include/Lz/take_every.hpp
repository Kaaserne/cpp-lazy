#pragma once

#ifndef LZ_TAKE_EVERY_HPP
#define LZ_TAKE_EVERY_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/take_every.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Takes every `offset` element from the iterable, starting from `start`. Returns the same iterator category as the input
 * iterable. Contains a size() method if the input iterable is sized.
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
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4 };
 * auto take_every = lz::take_every(vec, 2); // 1, 3
 * auto take_every = lz::take_every(vec, 2, 1); // 2, 4
 *
 * // or
 *
 * auto take_every = vec | lz::take_every(2); // 1, 3
 * auto take_every = vec | lz::take_every(2, 1); // 2, 4
 * ```
 */
LZ_INLINE_VAR constexpr detail::take_every_adaptor take_every{};

/**
 * @brief Take every iterable helper alias
 * @tparam Iterable Type of the iterable to take every element from
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4 };
 * lz::take_every_iterable<std::vector<int>> take_every = lz::take_every(vec, 2); // 1, 3
 * ```
 */
template<class Iterable>
using take_every_iterable = detail::take_every_iterable<Iterable>;

} // namespace lz

#endif
