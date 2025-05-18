#pragma once

#ifndef LZ_INTERLEAVE_HPP
#define LZ_INTERLEAVE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/interleave.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Interleave the elements of the given iterables. This means it returns the element of the first iterable, then the
 * second, etc... and resets when the last iterable is reached. The iterator will stop at the end of the shortest iterable. The
 * iterator category is the same as its "weakest" input iterable. It returns a sentinel if the input iterable has a forward
 * iterator. If its input iterables all have a .size() method, then this iterable will also have a .size() method. If the input
 * iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is traversed to get its
 * end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`. So, all in all: use lz::cache_size
 * if:
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
 * combinations) Example:
 * ```cpp
 * std::vector<int> vec1 = { 1, 2, 3 }, vec2 = { 4, 5, 6, 7 }, vec3 = { 8, 9, 10, 11, 12 };
 * auto interleaved = lz::interleave(vec1, vec2, vec3); // interleaved = { 1, 4, 8, 2, 5, 9, 3, 6, 10 }
 * // or
 * auto interleaved = vec1 | lz::interleave(vec2, vec3); // interleaved = { 1, 4, 8, 2, 5, 9, 3, 6, 10 }
 * ```
 */
constexpr detail::interleave_adaptor interleave{};

#else

/**
 * @brief Interleave the elements of the given iterables. This means it returns the element of the first iterable, then the
 * second, etc... and resets when the last iterable is reached. The iterator will stop at the end of the shortest iterable. The
 * iterator category is the same as its "weakest" input iterable. It returns a sentinel if the input iterable has a forward
 * iterator. If its input iterables all have a .size() method, then this iterable will also have a .size() method. If the input
 * iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is traversed to get its
 * end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`. So, all in all: use lz::cache_size
 * if:
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
 * combinations) Example:
 * ```cpp
 * std::vector<int> vec1 = { 1, 2, 3 }, vec2 = { 4, 5, 6, 7 }, vec3 = { 8, 9, 10, 11, 12 };
 * auto interleaved = lz::interleave(vec1, vec2, vec3); // interleaved = { 1, 4, 8, 2, 5, 9, 3, 6, 10 }
 * // or
 * auto interleaved = vec1 | lz::interleave(vec2, vec3); // interleaved = { 1, 4, 8, 2, 5, 9, 3, 6, 10 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::interleave_adaptor interleave{};

#endif

using detail::interleave_iterator;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_INTERLEAVE_HPP
