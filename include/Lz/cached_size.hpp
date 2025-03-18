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
 * only useful for non sized iterables and iterables that are bidirectional or less. The following iterables require a sized
 * iterable:
 * - `lz::chunks`
 * - `lz::enumerate`
 * - `lz::except`
 * - `lz::exclude`
 * - `lz::take_every`
 * - `lz::zip_longest`
 * - `lz::zip`
 *
 * ```cpp
 * auto to_filter = lz::range(10); // {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
 * auto filtered = to_filter | lz::filter([](int i) { return i % 2 == 0; }); // filter isn't sized
 *
 * // Get the size first, then chunks and enumerate will use the cached size
 * auto iterable = filtered | lz::cache_size | lz::chunks(3) | lz::enumerate;
 *
 * // iterable = { {0, {0, 2, 4}}, {1, {6, 8}} }
 * ```
 */
static constexpr detail::cached_size_adaptor cache_size{};

#else

/**
 * @brief Creates an iterable with a size. Gets the size eagerly (using `lz::eager_size`), so if the input iterable is
 * bidirectional or less and is not sized, the entire iterable will be traversed. This can be handy if you want to
 * cache the size of an iterable. For instance, if you want to know the size multiple times. Also some lz iterables require an
 * (eager_)sized iterable. If you use multiple of these, it can be handy to cache the size. Please bear in mind that this is
 * only useful for non sized iterables and iterables that are bidirectional or less. The following iterables require a sized
 * iterable:
 * - `lz::chunks`
 * - `lz::enumerate`
 * - `lz::except`
 * - `lz::exclude`
 * - `lz::take_every`
 * - `lz::zip_longest`
 * - `lz::zip`
 *
 * ```cpp
 * auto to_filter = lz::range(10); // {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
 * auto filtered = to_filter | lz::filter([](int i) { return i % 2 == 0; }); // filter isn't sized
 *
 * // Get the size first, then chunks and enumerate will use the cached size
 * auto iterable = filtered | lz::cache_size | lz::chunks(3) | lz::enumerate;
 *
 * // iterable = { {0, {0, 2, 4}}, {1, {6, 8}} }
 * ```
 */
LZ_INLINE_VAR constexpr detail::cached_size_adaptor cache_size{};

using detail::cached_size_iterable;

#endif

} // namespace lz

#endif
