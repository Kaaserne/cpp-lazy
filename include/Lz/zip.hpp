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
 * iterables. Example:
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
static constexpr detail::zip_adaptor detail::zip_adaptor::zip{};

#else

// TODO add cached_size if that one is finished in docs example

/**
 * @brief Zips two or more iterables together. If the sizes of the iterables are different, the shortest one will be used. It
 * contains a size() method if all the iterables have a size() method. It is the same iterator category as the 'weakest' of the
 * input iterables. If the weakest is a forward iterator, then the end() function will return the same types as its input
 * iterables. Example:
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

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif
