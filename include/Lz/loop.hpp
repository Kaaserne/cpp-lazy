#pragma once

#ifndef LZ_LOOP_HPP
#define LZ_LOOP_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/loop.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Loops over an iterable. Can be finite or infinite.
 * If finite:
 *     Contains a .size() method if the input iterable has a .size() method. Will return an actual iterator if the input
 *     iterable is at least bidirectional. Otherwise it will return a default_sentinel. Its input iterator category will
 *     be the same as the input iterator category. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4 };
 * auto looper = lz::loop(vec, 2); // {1, 2, 3, 4, 1, 2, 3, 4}
 * // or
 * auto looper = vec | lz::loop(2); // {1, 2, 3, 4, 1, 2, 3, 4}
 * ```
 * If infinite:
 *     Does not contain a .size() method. Loop infinitely over the input iterable. Its input iterator category will
 *     always be forward. It also returns a default_sentinel. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4 };
 * auto looper = lz::loop(vec); // {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, ...}
 * // or
 * auto looper = vec | lz::loop; // {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, ...}
 * ```
 */
constexpr detail::loop_adaptor loop{};

#else

/**
 * @brief Loops over an iterable. Can be finite or infinite.
 * If finite:
 *     Contains a .size() method if the input iterable has a .size() method. Will return an actual iterator if the input
 *     iterable is at least bidirectional. Otherwise it will return a default_sentinel. Its input iterator category will
 *     be the same as the input iterator category. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4 };
 * auto looper = lz::loop(vec, 2); // {1, 2, 3, 4, 1, 2, 3, 4}
 * // or
 * auto looper = vec | lz::loop(2); // {1, 2, 3, 4, 1, 2, 3, 4}
 * ```
 * If infinite:
 *     Does not contain a .size() method. Loop infinitely over the input iterable. Its input iterator category will
 *     always be forward. It also returns a default_sentinel. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4 };
 * auto looper = lz::loop(vec); // {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, ...}
 * // or
 * auto looper = vec | lz::loop; // {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, ...}
 * ```
 */
LZ_INLINE_VAR constexpr detail::loop_adaptor loop{};

#endif

using detail::loop_iterable;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_LOOP_HPP
