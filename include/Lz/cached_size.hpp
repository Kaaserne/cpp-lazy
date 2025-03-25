#pragma once

#ifndef LZ_CACHED_SIZE_HPP
#define LZ_CACHED_SIZE_HPP

#include <Lz/detail/adaptors/cached_size.hpp>

namespace lz {

#ifdef LZ_HAS_CXX_11

/**
 * @brief Creates an iterable with a size. Gets the size eagerly (using `lz::eager_size`), so if the input iterable is
 * bidirectional or less and is not sized, the entire iterable will be traversed. This can be handy if you want to
 * cache the size of an iterable. For instance, if you want to know the size multiple times. Also some lz iterables require an
 * (eager_)sized iterable. If you use multiple of these, it can be handy to cache the size. Please bear in mind that this is
 * only useful for non sized iterables and iterables that are exactly bidirectional (so forward excluded). The following iterables
 * require the size/distance of an iterable:
 * - `lz::chunks`
 * - `lz::enumerate`
 * - `lz::exclude`
 * - `lz::take`
 * - `lz::take_every`
 * - `lz::zip_longest`
 * - `lz::zip`
 * The size will be calculated when calling begin() or end() (or begin(), or end(), so not both) on the iterable. So if you call
 * begin()/end() multiple times, the size will be calculated multiple times.
 * ```cpp
 * auto to_filter = lz::range(10); // {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
 * auto filtered = to_filter | lz::filter([](int i) { return i % 2 == 0; }); // filter isn't sized
 *
 * // Get the size first, then chunks and enumerate will use the cached size
 * auto iterable = filtered | lz::cache_size | lz::chunks(3) | lz::enumerate;
 *
 * // iterable = { {0, {0, 2, 4}}, {1, {6, 8}} }
 * ```
 * So, all in all: use lz::cache_size if:
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
 * Are planning to call begin() or end() multiple times
 */
constexpr detail::cached_size_adaptor cache_size{};

#else

/**
 * @brief Creates an iterable with a size. Gets the size eagerly (using `lz::eager_size`), so if the input iterable is
 * bidirectional or less and is not sized, the entire iterable will be traversed. This can be handy if you want to
 * cache the size of an iterable. For instance, if you want to know the size multiple times. Also some lz iterables require an
 * (eager_)sized iterable. If you use multiple of these, it can be handy to cache the size. Please bear in mind that this is
 * only useful for non sized iterables and iterables that are exactly bidirectional (so forward excluded). The following iterables
 * require the size/distance of an iterable:
 * - `lz::chunks`
 * - `lz::enumerate`
 * - `lz::exclude`
 * - `lz::take`
 * - `lz::take_every`
 * - `lz::zip_longest`
 * - `lz::zip`
 * The size will be calculated when calling begin() or end() (or begin(), or end(), so not both) on the iterable. So if you call
 * begin()/end() multiple times, the size will be calculated multiple times.
 * ```cpp
 * auto to_filter = lz::range(10); // {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
 * auto filtered = to_filter | lz::filter([](int i) { return i % 2 == 0; }); // filter isn't sized
 *
 * // Get the size first, then chunks and enumerate will use the cached size
 * auto iterable = filtered | lz::cache_size | lz::chunks(3) | lz::enumerate;
 *
 * // iterable = { {0, {0, 2, 4}}, {1, {6, 8}} }
 * ```
 * So, all in all: use lz::cache_size if:
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
 * Are planning to call begin() or end() multiple times
 */
LZ_INLINE_VAR constexpr detail::cached_size_adaptor cache_size{};

using detail::cached_size_iterable;

#endif

} // namespace lz

#endif
