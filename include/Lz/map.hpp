#pragma once

#ifndef LZ_MAP_HPP
#define LZ_MAP_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/map.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief This adaptor is used to apply a function to each element in an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator or has a sentinel.
 * If its input iterable has a .size() method, then this iterable will also have a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto map = vec | lz::map([](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
 * // or
 * auto map = lz::map(vec, [](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::map_adaptor map{};

/**
 * @brief Map iterable helper alias.
 * @tparam Iterable The type of the iterable to map.
 * @tparam UnaryOp The type of the unary operation to apply to each element.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * lz::map_iterable<std::vector<int>, std::function<int(int)>> map_vec(vec, [](int i) { return i * 2; });
 * ```
 */
template<class Iterable, class UnaryOp>
using map_iterable = detail::map_iterable<Iterable, UnaryOp>;

} // namespace lz

#endif
