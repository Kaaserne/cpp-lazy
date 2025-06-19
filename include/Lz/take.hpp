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
 * - returns a default_sentinel if the input iterable is not bidirectional or has a sentinel
 * - if `n` is larger than `lz::eager_size(input_iterable)` then the total iterable size is equal to
 *   `lz::eager_size(input_iterable)`, thus preventing out of bounds. Traverses the entire iterable while taking the first `n`
 *   for non random access iterables.
 * - If the input iterable is exactly bidirectional, an iterable and not sized (like `lz::filter` for example), the entire
 *   sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`.
 *   So, all in all:
 *   use lz::cache_size if:
 *  - Your iterable is exactly bidirectional (so forward/random access excluded) and
 *  - Your iterable is not sized and
 *  - You either use multiple/a combination of the following iterables OR (see last point):
 *  - `lz::chunks`
 *  - `lz::enumerate`
 *  - `lz::exclude`
 *  - `lz::interleave`
 *  - `lz::rotate`
 *  - `lz::take`
 *  - `lz::take_every`
 *  - `lz::zip_longest`
 *  - `lz::zip`
 *  - Are planning call end() multiple times on the same instance (with one or more of the above iterable
 *    combinations).
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