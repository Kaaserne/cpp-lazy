#pragma once

// TODO interleaved inmplementation?

#ifndef LZ_INTERSECTION_HPP
#define LZ_INTERSECTION_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/intersection.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Intersects the first iterable with the second iterable. The result is a new iterable containing the elements that are in
 * both iterables. Both iterables must be sorted first. Returns a bidirectional iterable if the input iterables are at least
 * bidirectional, otherwise forward. Returns a sentinel if it is a forward iterable. Does not contain a .size() method. Example:
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
static constexpr detail::intersection_adaptor detail::intersection_adaptor::intersection{};

#else

/**
 * @brief Intersects the first iterable with the second iterable. The result is a new iterable containing the elements that are in
 * both iterables. Both iterables must be sorted first. Returns a bidirectional iterable if the input iterables are at least
 * bidirectional, otherwise forward. Returns a sentinel if it is a forward iterable. Does not contain a .size() method. Example:
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

#endif

using detail::intersection_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif