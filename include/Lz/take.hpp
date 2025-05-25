#pragma once

#ifndef LZ_TAKE_HPP
#define LZ_TAKE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/take.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to take the first n elements of an iterable or iterator. If an iterator is used, then you have to
 * be careful that the iterator yields at least `n` elements. If an iterable is used, then the library will check if the iterable
 * holds at least `n` elements and will yield min(size(), n) elements. The iterator category is the same as the input iterator
 * category. Its end() function will return a sentinel, if the input iterable has a forward iterator. Has a .size() method that
 * essentially returns `n` for iterators or `min(n, lz::eager_size(input_iterable))` for iterables. If the input
 * iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is traversed to get its
 * end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`. For `lz::take`, this is only relevant
 * for iterables, not for iterators. So, all in all: use lz::cache_size if:
 * - Your iterable is exactly bidirectional (so forward/random access excluded) and
 * - Your iterable is not sized and
 * - You either use multiple/a combination of the following iterables OR (see last point):
 * - `lz::chunks`
 * - `lz::enumerate`
 * - `lz::exclude`
 * - `lz::interleave`
 * - `lz::take`
 * - `lz::take_every`
 * - `lz::zip_longest`
 * - `lz::zip`
 * - Are planning to call begin() or end() multiple times on the same instance (with one or more of the above iterable
 * combinations). Example:
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
 * // auto res = lz::take(vec.begin(), 20); // out of bounds
 * ```
 */
constexpr detail::take_adaptor take{};

#else

/**
 * @brief This adaptor is used to take the first n elements of an iterable or iterator. If an iterator is used, then you have to
 * be careful that the iterator yields at least `n` elements. If an iterable is used, then the library will check if the iterable
 * holds at least `n` elements and will yield min(size(), n) elements. The iterator category is the same as the input iterator
 * category. Its end() function will return a sentinel, if the input iterable is a forward iterator. Has a .size() method that
 * essentially returns `n` for iterators or `min(n, lz::eager_size(input_iterable))` for iterables. If the input
 * iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is traversed to get its
 * end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`. For `lz::take`, this is only relevant
 * for iterables, not for iterators. For iterators, next(iterator, n) is used to get the end. So, all in all: use
 * lz::cache_size if:
 * - Your iterable is exactly bidirectional (so forward/random access excluded) and
 * - Your iterable is not sized and
 * - You either use multiple/a combination of the following iterables OR (see last point):
 * - `lz::chunks`
 * - `lz::enumerate`
 * - `lz::exclude`
 * - `lz::interleave`
 * - `lz::take`
 * - `lz::take_every`
 * - `lz::zip_longest`
 * - `lz::zip`
 * - Are planning to call begin() or end() multiple times on the same instance (with one or more of the above iterable
 * combinations). Example:
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
 * // auto res = lz::take(vec.begin(), 20); // out of bounds
 * ```
 */
LZ_INLINE_VAR constexpr detail::take_adaptor take{};

#endif

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