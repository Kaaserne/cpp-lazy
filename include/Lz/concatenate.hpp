#pragma once

#ifndef LZ_CONCATENATE_HPP
#define LZ_CONCATENATE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/concatenate.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Concatenates multiple iterables into one iterable. The reference type returned by operator* will:
 * - be by value if one of the iterables yields by value
 * - be by const reference if one of the iterables yield by const reference.
 * - be by mutable reference if all iterables yield by mutable reference.
 * Contains a .size() function if all iterables have a .size()function. The size is the sum of all the sizes of the iterables.
 * Contains a sentinel if one of the iterables contains a sentinel. Its iterator category is the 'weakest' of the input
 * iterables. Example:
 * ```cpp
 * std::vector<int> a = {1, 2};
 * std::vector<int> b = {3, 4};
 * auto concatenated = lz::concat(a, b); // concatenated = {1, 2, 3, 4}
 * // or
 * auto concatenated = a | lz::concat(b); // concatenated = {1, 2, 3, 4}
 * auto range = lz::range(5, 10);
 * auto c = lz::concat(a, range); // yields by value, not by reference
 * ```
 */
LZ_INLINE_VAR constexpr detail::concatenate_adaptor concat{};

/**
 * @brief Helper alias for the concatenate iterable.
 * @tparam Iterables The iterables to concatenate.
 * ```cpp
 * std::vector<int> a = {1, 2};
 * std::vector<int> b = {3, 4};
 * lz::concatenate_iterable<std::vector<int>, std::vector<int>> concatenated = lz::concat(a, b);
 * ```
 */
template<class... Iterables>
using concatenate_iterable = detail::concatenate_iterable<Iterables...>;

} // namespace lz

#endif // LZ_CONCATENATE_HPP