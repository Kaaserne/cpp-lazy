#pragma once

#ifndef LZ_TAKE_EVERY_HPP
#define LZ_TAKE_EVERY_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/take_every.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Takes every `offset` element from the iterable, starting from `start`. Returns the same iterator category as the input
 * iterable. Contains a size() method if the input iterable is sized. If input iterable is forward or less, the end() method will
 * return a default sentinel. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the
 * entire sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use
 * `lz::cache_size`.
 * So, all in all: use lz::cache_size if:
 * - Your iterable is exactly bidirectional (so forward excluded)
 * - Your iterable is not sized
 * - You use multiple/a combination of the following iterables:
 *   - lz::chunks
 *   - lz::enumerate
 *   - lz::exclude
 *   - lz::take_every
 *   - lz::zip_longest
 *   - lz::zip
 * Are planning to call begin() or end() multiple times
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
static constexpr detail::take_every_adaptor detail::take_every_adaptor::take_every{};

#else

/**
 * @brief Takes every `offset` element from the iterable, starting from `start`. Returns the same iterator category as the input
 * iterable. Contains a size() method if the input iterable is sized. If input iterable is forward or less, the end() method will
 * return a default sentinel. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the
 * entire sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use
 * `lz::cache_size`.
 * So, all in all: use lz::cache_size if:
 * - Your iterable is exactly bidirectional (so forward excluded)
 * - Your iterable is not sized
 * - You use multiple/a combination of the following iterables:
 *   - lz::chunks
 *   - lz::enumerate
 *   - lz::exclude
 *   - lz::take_every
 *   - lz::zip_longest
 *   - lz::zip
 * Are planning to call begin() or end() multiple times
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

#endif // LZ_HAS_CXX_11

using detail::take_every_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif