#pragma once

#ifndef LZ_ZIP_LONGEST_HPP
#define LZ_ZIP_LONGEST_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/zip_longest.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Zips multiple iterables together. If one of the iterables is shorter than the others, it will return an empty optional
 * instead of a non empty optional. The optional contains a tuple of `std::reference_wrapper`s to the elements itself if it is not
 * empty. Contains a size() method if all the iterables are sized. Will return the size of the largest iterable. Its iterator
 * category is the same as the iterator category of the 'weakest' iterator. Its end() function will return a default_sentinel if
 * the iterator is not bidirectional. Example:
 * ```cpp
 * std::vector<int> a = { 1, 2, 3 };
 * std::vector<int> b = { 4, 5 };
 * auto zipped = lz::zip_longest(a, b); // {(1, 4), (2, 5), (3, lz::nullopt)}
 * // or
 * auto zipped = a | lz::zip_longest(b); // {(1, 4), (2, 5), (3, lz::nullopt)}
 * ```
 */
static constexpr detail::zip_longest_adaptor detail::zip_longest_adaptor::zip_longest{};

#else

/**
 * @brief Zips multiple iterables together. If one of the iterables is shorter than the others, it will return an empty optional
 * instead of a non empty optional. The optional contains a tuple of `std::reference_wrapper`s to the elements itself if it is not
 * empty. Contains a size() method if all the iterables are sized. Will return the size of the largest iterable. Its iterator
 * category is the same as the iterator category of the 'weakest' iterator. Its end() function will return a default_sentinel if
 * the iterator is not bidirectional. Example:
 * ```cpp
 * std::vector<int> a = { 1, 2, 3 };
 * std::vector<int> b = { 4, 5 };
 * auto zipped = lz::zip_longest(a, b); // {(1, 4), (2, 5), (3, lz::nullopt)}
 * // or
 * auto zipped = a | lz::zip_longest(b); // {(1, 4), (2, 5), (3, lz::nullopt)}
 * ```
 */
LZ_INLINE_VAR constexpr detail::zip_longest_adaptor zip_longest{};

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_ZIP_LONGEST_HPP