#pragma once

#ifndef LZ_ZIP_HPP
#define LZ_ZIP_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/zip.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Zips two or more iterables together. If the sizes of the iterables are different, the shortest one will be used. It
 * contains a size() method if all the iterables have a size() method. It is the same iterator category as the 'weakest' of the
 * input iterables. If the weakest is a forward iterator, then the end() function will return the same types as its input
 * iterables. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is
 * traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`.
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
 * combinations). Example:
 * ```cpp
 * std::vector<int> a = { 1, 2, 3 };
 * std::vector<int> b = { 4, 5, 6 };
 * std::vector<int> c = { 7, 8, 9 };
 * auto zipped = lz::zip(a, b, c); // { (1, 4, 7), (2, 5, 8), (3, 6, 9) }
 *
 * // or
 *
 * auto zipped = a | lz::zip(b, c); // { (1, 4, 7), (2, 5, 8), (3, 6, 9) }
 *
 * std::vector<int> d = { 10 };
 * auto zipped2 = lz::zip(a, d); // { (1, 10) }
 * ```
 */
constexpr detail::zip_adaptor zip{};

#else

/**
 * @brief Zips two or more iterables together. If the sizes of the iterables are different, the shortest one will be used. It
 * contains a size() method if all the iterables have a size() method. It is the same iterator category as the 'weakest' of the
 * input iterables. If the weakest is a forward iterator, then the end() function will return the same types as its input
 * iterables. If the input iterable is exactly bidirectional and not sized (like `lz::filter` for example), the entire sequence is
 * traversed to get its end size (using `lz::eager_size`), so it may be worth your while to use `lz::cache_size`.
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
 * combinations) Example:
 * ```cpp
 * std::vector<int> a = { 1, 2, 3 };
 * std::vector<int> b = { 4, 5, 6 };
 * std::vector<int> c = { 7, 8, 9 };
 * auto zipped = lz::zip(a, b, c); // { (1, 4, 7), (2, 5, 8), (3, 6, 9) }
 *
 * // or
 *
 * auto zipped = a | lz::zip(b, c); // { (1, 4, 7), (2, 5, 8), (3, 6, 9) }
 *
 * std::vector<int> d = { 10 };
 * auto zipped2 = lz::zip(a, d); // { (1, 10) }
 * ```
 */
LZ_INLINE_VAR constexpr detail::zip_adaptor zip{};

#endif // LZ_HAS_CXX_11

using detail::zip_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif
