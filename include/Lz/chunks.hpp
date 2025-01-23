#pragma once

#ifndef LZ_CHUNKS_HPP
#define LZ_CHUNKS_HPP

#include <Lz/detail/adaptors/chunks.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to make chunks of the iterable, based on chunk size. The iterator
 * category is the same as its input iterable. It returns an iterable of iterables. Its end() function will return a sentinel,
 * if the input iterable has a forward iterator. If its input iterable has a .size() method, then this iterable will also have a
 * .size() method.
 *
 * Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto chunked = lz::chunks(vec, 3); // chunked = { {1, 2, 3}, {4, 5} }
 * // or
 * auto chunked = vec | lz::chunks(3); // chunked = { {1, 2, 3}, {4, 5} }
 * ```
 */
detail::chunks_adaptor detail::chunks_adaptor::chunks;

#else

/**
 * @brief This adaptor is used to make chunks of the iterable, based on chunk size. The iterator
 * category is the same as its input iterable. It returns an iterable of iterables. Its end() function will return a sentinel,
 * if the input iterable has a forward iterator. If its input iterable has a .size() method, then this iterable will also have a
 * .size() method.
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

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CHUNKS_HPP
