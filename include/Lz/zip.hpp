#pragma once

#ifndef LZ_ZIP_HPP
#define LZ_ZIP_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/zip.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Zips two or more iterables together. If the sizes of the iterables are different, the shortest one will be used. It
 * contains a size() method if all the iterables have a size() method. It is the same iterator category as the 'weakest' of the
 * input iterables. If the weakest is a forward iterator or one of them has a sentinel, then the end() function will return the
 * same types as its input iterables.
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
 * std::vector<int> a = { 1, 2, 3 };
 * std::vector<int> b = { 4, 5, 6 };
 * std::vector<int> c = { 7, 8, 9 };
 * auto zipped = lz::zip(a, b, c); // { (1, 4, 7), (2, 5, 8), (3, 6, 9) }
 *
 * // or
 *
 * auto zipped = a | lz::zip(b, c); // { (1, 4, 7), (2, 5, 8), (3, 6, 9) }
 *
 * std::vector<int> d = { 10 };
 * auto zipped2 = lz::zip(a, d); // { (1, 10) }
 * ```
 */
LZ_INLINE_VAR constexpr detail::zip_adaptor zip{};

/**
 * @brief Zip helper alias.
 * @tparam Iterables The iterables to zip together.
 * ```cpp
 * std::vector<int> a = { 1, 2, 3 };
 * std::vector<int> b = { 4, 5 };
 * lz::zip_iterable<std::vector<int>, std::vector<int>> zipped = lz::zip(a, b);
 * ```
 */
template<class... Iterables>
using zip_iterable = detail::zip_iterable<Iterables...>;

} // namespace lz

#endif
