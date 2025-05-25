#pragma once

#ifndef LZ_CHUNKS_HPP
#define LZ_CHUNKS_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/chunks.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to make chunks of the iterable, based on chunk size. The iterator
 * category is the same as its input iterable. It returns an iterable of iterables. Its end() function will return a sentinel,
 * if the input iterable has a forward iterator. If its input iterable has a .size() method, then this iterable will also have a
 * .size() method. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire
 * sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`.
 * So, all in all: use lz::cache_size if:
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
 * combinations)
 *
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto chunked = lz::chunks(vec, 3); // chunked = { {1, 2, 3}, {4, 5} }
 * // or
 * auto chunked = vec | lz::chunks(3); // chunked = { {1, 2, 3}, {4, 5} }
 * ```
 */
constexpr detail::chunks_adaptor chunks;

#else

/**
 * @brief This adaptor is used to make chunks of the iterable, based on chunk size. The iterator
 * category is the same as its input iterable. It returns an iterable of iterables. Its end() function will return a sentinel,
 * if the input iterable has a forward iterator. If its input iterable has a .size() method, then this iterable will also have a
 * .size() method. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire
 * sequence is traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`.
 * So, all in all: use lz::cache_size if:
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
 * combinations)
 *
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto chunked = lz::chunks(vec, 3); // chunked = { {1, 2, 3}, {4, 5} }
 * // or
 * auto chunked = vec | lz::chunks(3); // chunked = { {1, 2, 3}, {4, 5} }
 * ```
 */
LZ_INLINE_VAR constexpr detail::chunks_adaptor chunks{};

#endif // LZ_HAS_CXX_11

/**
 * @brief This is the type of the iterable returned by `lz::chunks`.
 * @tparam Iterable The type of the input iterable.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * lz::chunks_iterable<std::vector<int>> chunked = lz::chunks(vec, 3);
 * ```
 */
template<class Iterable>
using chunks_iterable = detail::chunks_iterable<Iterable>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CHUNKS_HPP
