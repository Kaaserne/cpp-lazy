#pragma once

#ifndef LZ_MAP_HPP
#define LZ_MAP_HPP

#include <Lz/detail/adaptors/map.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief This adaptor is used to apply a function to each element in an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. If its input
 * iterable has a .size() method, then this iterable will also have a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto map = vec | lz::map([](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
 * // or
 * auto map = lz::map(vec, [](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
 * ```
 */
constexpr detail::map_adaptor map{};

#else

/**
 * @brief This adaptor is used to apply a function to each element in an iterable. The iterator category is the same as the input
 * iterator category. Its end() function will return a sentinel, if the input iterable has a forward iterator. If its input
 * iterable has a .size() method, then this iterable will also have a .size() method. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto map = vec | lz::map([](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
 * // or
 * auto map = lz::map(vec, [](int i) { return i * 2; }); // map = { 2, 4, 6, 8, 10 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::map_adaptor map{};

#endif

using detail::map_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif