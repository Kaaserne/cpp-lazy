#pragma once

#ifndef LZ_INTERSECTION_HPP
#define LZ_INTERSECTION_HPP

#include <Lz/detail/adaptors/intersection.hpp>
#include <Lz/procs/chain.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Intersects the first iterable with the second iterable. The result is a new iterable containing the elements that are in
 * both iterables. Both iterables must be sorted first. Returns a bidirectional iterable if the input iterables are at least
 * bidirectional, otherwise forward. Returns a sentinel if it is a forward iterable or has a sentinel. Does not contain a .size()
 * method. Example:
 * ```cpp
 * std::string a = "aaaabbcccddee";
 * std::string b = "aabccce";
 *
 * std::sort(a.begin(), a.end());
 * std::sort(b.begin(), b.end());
 *
 * auto intersect = lz::intersection(a, b); // { 'a', 'a', 'b', 'c', 'c', 'c', 'e' }
 * // or
 * auto intersect = lz::intersection(a, b, std::less<>{}); // { 'a', 'a', 'b', 'c', 'c', 'c', 'e' }
 * // or
 * auto intersect = a | lz::intersection(b); // { 'a', 'a', 'b', 'c', 'c', 'c', 'e' }
 * // or
 * auto intersect = a | lz::intersection(b, std::less<>{}); // { 'a', 'a', 'b', 'c', 'c', 'c', 'e' }
 * ```
 */
LZ_INLINE_VAR constexpr detail::intersection_adaptor intersection{};

/**
 * @brief Intersection helper alias.
 *
 * @tparam Iterable The first iterable type.
 * @tparam Iterable2 The second iterable type.
 * @tparam BinaryPredicate The binary predicate type used to compare elements from both iterables. Defaults to `std::less<>`.
 * ```cpp
 * std::string a = "aaaabbcccddee";
 * std::string b = "aabccce";
 * std::sort(a.begin(), a.end());
 * std::sort(b.begin(), b.end());
 * lz::intersection_iterable<std::string, std::string> intersect = lz::intersection(a, b);
 * ```
 */
template<class Iterable, class Iterable2, class BinaryPredicate = LZ_BIN_OP(less, detail::val_iterable_t<Iterable>)>
using intersection_iterable = detail::intersection_iterable<Iterable, Iterable2, BinaryPredicate>;

} // namespace lz

#endif // LZ_INTERSECTION_HPP
