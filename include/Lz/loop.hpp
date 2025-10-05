#pragma once

#ifndef LZ_LOOP_HPP
#define LZ_LOOP_HPP

#include <Lz/procs/chain.hpp>
#include <Lz/detail/adaptors/loop.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Loops over an iterable. Can be finite or infinite.
 * If finite:
 *     Contains a .size() method if the input iterable has a .size() method. Will return an actual iterator if the input
 *     iterable is at least bidirectional and does not have a sentinel. Otherwise it will return a default_sentinel_t. Its input
 * iterator category will be the same as the input iterator category. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4 };
 * auto looper = lz::loop(vec, 2); // {1, 2, 3, 4, 1, 2, 3, 4}
 * // or
 * auto looper = vec | lz::loop(2); // {1, 2, 3, 4, 1, 2, 3, 4}
 * ```
 * If infinite:
 *     Does not contain a .size() method. Loop infinitely over the input iterable. Its input iterator category will
 *     always be forward. It returns a default_sentinel_t. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4 };
 * auto looper = lz::loop(vec); // {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, ...}
 * // or
 * auto looper = vec | lz::loop; // {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, ...}
 * ```
 */
LZ_INLINE_VAR constexpr detail::loop_adaptor loop{};

/**
 * @brief Loop infinite helper alias.
 * @tparam Iterable The iterable type.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4 };
 * lz::loop_iterable_inf loop = lz::loop(vec);
 * ```
 */
template<class Iterable>
using loop_iterable_inf = detail::loop_iterable<Iterable, true>;

/**
 * @brief Loop finite helper alias.
 * @tparam Iterable The iterable type.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4 };
 * lz::loop_iterable loop = lz::loop(vec, 2);
 * ```
 */
template<class Iterable>
using loop_iterable = detail::loop_iterable<Iterable, false>;

} // namespace lz

#endif // LZ_LOOP_HPP
