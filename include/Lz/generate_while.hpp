#pragma once

#ifndef LZ_GENERATE_WHILE_HPP
#define LZ_GENERATE_WHILE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/generate_while.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

/**
 * @brief Generates elements while the predicate returns true. The predicate must return an object that is compatible with
 * std::get. The first element (std::get<0>) must be an object convertible to bool, the second element (std::get<1>) can be any
 * type. This iterable does not contain a .size() member function. Its end() function returns a sentinel, rather than an actual
 * iterator object. Example:
 * ```cpp
 * int i = 0;
 * auto generator = lz::generate_while([&i]() {
 *    auto copy = i++;
 *    return std::make_pair(copy != 4, copy);
 * }); // { 0, 1, 2, 3 }
 * // or (cxx 14)
 * auto generator = lz::generate_while([i = 0]() {
 *   auto pair = std::make_pair(i != 4, i);
 *    ++i;
 *   return pair;
 * }); // { 0, 1, 2, 3 }
 * ```
 */
static constexpr detail::generate_while_adaptor detail::generate_while_adaptor::generate_while{};

#else

/**
 * @brief Generates elements while the predicate returns true. The predicate must return an object that is compatible with
 * std::get. The first element (std::get<0>) must be an object convertible to bool, the second element (std::get<1>) can be any
 * type. This iterable does not contain a .size() member function. Its end() function returns a sentinel, rather than an actual
 * iterator object. Example:
 * ```cpp
 * int i = 0;
 * auto generator = lz::generate_while([&i]() {
 *    auto copy = i++;
 *    return std::make_pair(copy != 4, copy);
 * }); // { 0, 1, 2, 3 }
 * // or (cxx 14)
 * auto generator = lz::generate_while([i = 0]() {
 *   auto pair = std::make_pair(i != 4, i);
 *    ++i;
 *   return pair;
 * }); // { 0, 1, 2, 3 }
 * ```
 *
 */
LZ_INLINE_VAR constexpr detail::generate_while_adaptor generate_while{};

#endif // LZ_HAS_CXX_11

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif