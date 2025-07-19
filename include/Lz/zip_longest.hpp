#pragma once

#ifndef LZ_ZIP_LONGEST_HPP
#define LZ_ZIP_LONGEST_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/zip_longest.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Zips two or more iterables together. If one of the iterables is shorter than the others, it will return an empty
 * optional instead of a non empty optional. The optional contains a tuple of `std::reference_wrapper`s to the elements itself if
 * it is not empty. Contains a size() method if all the iterables are sized. Will return the size of the largest iterable. Its
 * iterator category is the same as its 'weakest' input iterables. Returns a sentinel if one of the iterables has a sentinel.
 *
 * If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is traversed
 * to get its end size (using `lz::eager_size`); this can be inefficient. To prevent this traversal alltogether, you can use
 * `lz::iter_decay` defined in `<Lz/iter_tools.hpp>` or you can use `lz::cache_size` to cache the size of the iterable.
 * `lz::iter_decay` decays the iterable into a forward one and since forward iterators cannot go backward, its entire size is
 * therefore also not needed to create an iterator from its end() funciton. `lz::cache_size` however will traverse the iterable
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
 * std::vector<int> b = { 4, 5 };
 * auto zipped = lz::zip_longest(a, b); // {(1, 4), (2, 5), (3, lz::nullopt)}
 * // or
 * auto zipped = a | lz::zip_longest(b); // {(1, 4), (2, 5), (3, lz::nullopt)}
 *
 * auto filter = lz::filter(a, [](int i) { return i % 2 == 0; });
 * auto zipped = lz::zip_longest(a, filter); // {(1, lz::nullopt), (2, 2), (3, lz::nullopt)}
 * // zipped will be bidirectional. However, the entire sequence of `filter` will be traversed in order to get the size of the
 * // iterable. For `a` however, `a.size()` is used.
 * ```
 */
LZ_INLINE_VAR constexpr detail::zip_longest_adaptor zip_longest{};

/**
 * @brief Zip longest helper alias.
 * @tparam Iterables The iterables to zip together.
 * ```cpp
 * std::vector<int> a = { 1, 2, 3 };
 * std::vector<int> b = { 4, 5 };
 * lz::zip_longest_iterable<std::vector<int>, std::vector<int>> zipped = lz::zip_longest(a, b);
 * ```
 */
template<class... Iterables>
using zip_longest_iterable = detail::zip_longest_iterable<Iterables...>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_ZIP_LONGEST_HPP