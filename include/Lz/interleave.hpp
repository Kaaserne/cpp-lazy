#pragma once

#ifndef LZ_INTERLEAVE_HPP
#define LZ_INTERLEAVE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/interleave.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Interleave the elements of the given iterables. This means it returns the element of the first iterable, then the
 * second, etc... and resets when the last iterable is reached. The iterator will stop at the end of the shortest iterable.
 * The iterator category is the same as its "weakest" input iterable. The reference type returned by operator* will:
 * - be by value if one of the iterables yields by value
 * - be by const reference if one of the iterables yield by const reference.
 * - be by mutable reference if all iterables yield by mutable reference.
 * Contains a .size() function if all iterables have a .size() function. The size is the sum of all the sizes of the
 * iterables. Contains a sentinel if one of the iterables contains a sentinel. Its iterator category is the 'weakest' of the
 * input iterables. Contains a sentinel if one of the iterables contains a sentinel or if one of them is forward.
 *
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
 * std::vector<int> vec1 = { 1, 2, 3 }, vec2 = { 4, 5, 6, 7 }, vec3 = { 8, 9, 10, 11, 12 };
 * auto interleaved = lz::interleave(vec1, vec2, vec3); // interleaved = { 1, 4, 8, 2, 5, 9, 3, 6, 10 }
 * // or
 * auto interleaved = vec1 | lz::interleave(vec2, vec3); // interleaved = { 1, 4, 8, 2, 5, 9, 3, 6, 10 }
 * auto c = lz::interleave(vec1, lz::range(5, 10)); // yields by value, not by reference
 * ```
 */
LZ_INLINE_VAR constexpr detail::interleave_adaptor interleave{};


/**
 * @brief Interleave alias helper.
 * @tparam Iterables The iterables to interleave.
 * ```
 * std::vector<int> vec1 = { 1, 2, 3 }, vec2 = { 4, 5, 6, 7 };
 * lz::interleave_iterable<std::vector<int>, std::vector<int>> interleaved = lz::interleave(vec1, vec2);
 * ```
*/
template<class... Iterables>
using interleave_iterable = detail::interleave_iterable<Iterables...>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_INTERLEAVE_HPP
