#pragma once

#ifndef LZ_REVERSE_HPP
#define LZ_REVERSE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/reverse.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Reverses the iterable. Contains a size method if the iterable has a size method. Example:
 * ```cpp
 * std::vector<int> v = { 1, 2, 3, 4, 5 };
 * auto reversed = lz::reverse(v); // { 5, 4, 3, 2, 1 }
 * // or
 * auto reversed = v | lz::reverse; // { 5, 4, 3, 2, 1 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::reverse_adaptor<false> reverse{};

/**
 * @brief Reverses the iterable with a caching mechanism. Normally, the std::reverse_iterator uses an operator-- in its operator*
 * to access the previous element, which can be inefficient for some iterables such as iterables that traverses multiple elements
 * in its operator-- (such as `lz::filter` or `lz::take_every`). Say filter_iterator::operator-- traverses 10 elements to get
 * to the previous element, then this means 20 elements will be traversed when operator-- and operator* are called in a row. This
 * iterable prevents that by using a caching mechanism.
 * ```cpp
 * std::vector<int> v = // very large vector ranging from 0 to 10.000.000
 * auto reversed_slow = v | lz::filter([](int i) { return i < 3; }) | lz::reverse; // Traverses 20.000.000 items.
 * auto reversed = v | lz::filter([](int i) { return i < 3; }) | lz::cached_reverse; // Traverses only 10.000.000 items.
 * ```
 */
LZ_INLINE_VAR constexpr detail::reverse_adaptor<true> cached_reverse{};

/**
 * @brief Reverse iterable helper alias.
 * @tparam Iterable The type of the iterable to reverse.
 * ```cpp
 * std::vector<int> v = { 1, 2, 3, 4, 5 };
 * lz::reverse_iterable<std::vector<int>> reversed = lz::reverse(v);
 * ```
 */
template<class Iterable>
using reverse_iterable = detail::reverse_iterable<Iterable, false>;

template<class Iterable>
using cached_reverse_iterable = detail::reverse_iterable<Iterable, true>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_REVERSE_HPP
