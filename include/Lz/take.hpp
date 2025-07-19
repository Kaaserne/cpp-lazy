#pragma once

#ifndef LZ_TAKE_HPP
#define LZ_TAKE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/take.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief This adaptor is used to take the first n elements of an iterable or iterator. The iterator category is the same as
 * the input iterator category. Has a .size() method that essentially returns
 * `n` or min(n, size(input_iterable)).

 * If the parameter is an iterable:
 * - returns a default_sentinel_t if the input iterable is not bidirectional or has a sentinel
 * - if `n` is larger than `lz::eager_size(input_iterable)` then the total iterable size is equal to
 *   `lz::eager_size(input_iterable)`, thus preventing out of bounds. Traverses the entire iterable while taking the first `n`
 *   for non random access iterables.
 * - If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is
 traversed
 *   to get its end size (using `lz::eager_size`); this can be inefficient. To prevent this traversal alltogether, you can use
 *   `lz::iter_decay` defined in `<Lz/iter_tools.hpp>` or you can use `lz::cache_size` to cache the size of the iterable.
 *   `lz::iter_decay` decays the iterable into a forward one and since forward iterators cannot go backward, its entire size is
 *   therefore also not needed to create an iterator from its end() funciton. `lz::cache_size` however will traverse the iterable
 *   once and cache the size, so that subsequent calls to `end()` will not traverse the iterable again, but will return the cached
 *   size instead. The following iterables require a(n) (eagerly)sized iterable:
 *   - `lz::chunks`
 *   - `lz::enumerate`
 *   - `lz::exclude`
 *   - `lz::interleave`
 *   - `lz::rotate`
 *   - `lz::take`
 *   - `lz::take_every`
 *   - `lz::zip_longest`
 *   - `lz::zip`
 *
 * If the parameter is an iterator:
 * - if `n` is larger than the actual size if the iterator then this is undefined behaviour.
 * - `lz::eager_size` is not used, so it is not guaranteed that the iterator will not go out of bounds.
 * - never returns a sentinel
 *
 * Example:
 * ```cpp
 * auto vec = std::vector<int>{1, 2, 3, 4, 5};
 * auto res = lz::take(vec, 2); // res = {1, 2}
 * // or
 * auto res = vec | lz::take(2); // res = {1, 2}
 * // or
 * auto res = lz::take(vec, 20); // res = {1, 2, 3, 4, 5}
 * // or
 * auto res = vec | lz::take(20); // res = {1, 2, 3, 4, 5}
 * auto res = lz::take(vec.begin(), 2); // res = {1, 2}
 * // auto res = lz::take(vec.begin(), 20); // undefined behaviour, as the iterator will go out of bounds
 * // vec.begin() | lz::take(2); // Not supported, as piping only works for iterables, not for iterators
 * ```
 */
LZ_INLINE_VAR constexpr detail::take_adaptor take{};

/**
 * @brief This is a helper alias that takes an iterable or iterator and returns a take_iterable.
 * @tparam IteratorOrIterable The type of the iterable or iterator. 
 * ```cpp
 * auto vec = std::vector<int>{1, 2, 3, 4, 5};
 * lz::take_iterable<std::vector<int>> res = lz::take(vec, 2); // res = {1, 2}
 * // or
 * lz::take_iterable<std::vector<int>::iterator> res = lz::take(vec.begin(), 2); // res = {1, 2}
 * ```
 */
template<class IteratorOrIterable>
using take_iterable = detail::take_iterable<IteratorOrIterable>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif