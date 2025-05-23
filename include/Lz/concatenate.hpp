#pragma once

#ifndef LZ_CONCATENATE_HPP
#define LZ_CONCATENATE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/concatenate.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Concatenates multiple iterables into one iterable. Contains a .size() function if all iterables have a .size() function.
 * The size is the sum of all the sizes of the iterables. Contains a sentinel if one of the iterables contains a sentinel. Its
 * iterator category is the 'weakest' of the input iterables. Example:
 * ```cpp
 * std::vector<int> a = {1, 2};
 * std::vector<int> b = {3, 4};
 * auto concatenated = lz::concat(a, b); // concatenated = {1, 2, 3, 4}
 * // or
 * auto concatenated = a | lz::concat(b); // concatenated = {1, 2, 3, 4}
 * ```
 */
constexpr detail::concatenate_adaptor concat{};

#else

/**
 * @brief Concatenates multiple iterables into one iterable. Contains a .size() function if all iterables have a .size() function.
 * The size is the sum of all the sizes of the iterables. Contains a sentinel if one of the iterables contains a sentinel. Its
 * iterator category is the 'weakest' of the input iterables. Example:
 * ```cpp
 * std::vector<int> a = {1, 2};
 * std::vector<int> b = {3, 4};
 * auto concatenated = lz::concat(a, b); // concatenated = {1, 2, 3, 4}
 * // or
 * auto concatenated = a | lz::concat(b); // concatenated = {1, 2, 3, 4}
 * ```
 */
LZ_INLINE_VAR constexpr detail::concatenate_adaptor concat{};

#endif

using detail::concatenate_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_CONCATENATE_HPP