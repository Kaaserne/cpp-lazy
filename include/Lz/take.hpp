#pragma once

#ifndef LZ_TAKE_HPP
#define LZ_TAKE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/take.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to take the first n elements of an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. Has a .size()
 * method that essentially returns `n`. If `n` is larger than `lz::eager_size(input_iterable)` then
 * `lz::eager_size(input_iterable` is used, thus preventing out of bounds. If the input iterable is exactly bidirectional and not
 * sized (like `lz::filter` for example), the entire sequence is traversed to get its end size (using `lz::eager_size`), so it may
 * be worth your while to use `lz::cache_size`. So, all in all: use
 * lz::cache_size if:
 * - Your iterable is exactly bidirectional (so forward excluded)
 * - Your iterable is not sized
 * - You use multiple/a combination of the following iterables:
 *   - lz::chunks
 *   - lz::enumerate
 *   - lz::exclude
 *   - lz::take
 *   - lz::take_every
 *   - lz::zip_longest
 *   - lz::zip
 * Are planning to call begin() or end() multiple times. Example:
 * ```cpp
 * auto vec = std::vector<int>{1, 2, 3, 4, 5};
 * auto res = lz::take(vec, 2); // res = {1, 2}
 * // or
 * auto res = vec | lz::take(2); // res = {1, 2}
 * // or
 * auto res = lz::take(vec, 20); // res = {1, 2, 3, 4, 5}
 * // or
 * auto res = vec | lz::take(20); // res = {1, 2, 3, 4, 5}
 * ```
 */
static constexpr detail::take_adaptor detail::take_adaptor::take{};

#else

/**
 * @brief This adaptor is used to take the first n elements of an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. Has a .size()
 * method that essentially returns `n`. If `n` is larger than `lz::eager_size(input_iterable)` then
 * `lz::eager_size(input_iterable` is used, thus preventing out of bounds. If the input iterable is exactly bidirectional and not
 * sized (like `lz::filter` for example), the entire sequence is traversed to get its end size (using `lz::eager_size`), so it may
 * be worth your while to use `lz::cache_size`. So, all in all: use
 * lz::cache_size if:
 * - Your iterable is exactly bidirectional (so forward excluded)
 * - Your iterable is not sized
 * - You use multiple/a combination of the following iterables:
 *   - lz::chunks
 *   - lz::enumerate
 *   - lz::exclude
 *   - lz::take
 *   - lz::take_every
 *   - lz::zip_longest
 *   - lz::zip
 * Are planning to call begin() or end() multiple times. Example:
 * ```cpp
 * auto vec = std::vector<int>{1, 2, 3, 4, 5};
 * auto res = lz::take(vec, 2); // res = {1, 2}
 * // or
 * auto res = vec | lz::take(2); // res = {1, 2}
 * // or
 * auto res = lz::take(vec, 20); // res = {1, 2, 3, 4, 5}
 * // or
 * auto res = vec | lz::take(20); // res = {1, 2, 3, 4, 5}
 * ```
 */
LZ_INLINE_VAR constexpr detail::take_adaptor take{};

#endif

using detail::take_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif