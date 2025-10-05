#pragma once

#ifndef LZ_GENERATE_WHILE_HPP
#define LZ_GENERATE_WHILE_HPP

#include <Lz/procs/chain.hpp>
#include <Lz/detail/adaptors/generate_while.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Generates elements while the predicate returns true. The predicate must return an object that is compatible with
 * std::get. The second element (std::get<1>) must be an object convertible to bool, the first element (std::get<0>) can be any
 * type. This iterable does not contain a .size() member function. Its end() function returns a sentinel, rather than an actual
 * iterator object. It returns a (std::)input_iterator(_tag). Example:
 * ```cpp
 * int i = 0;
 * auto generator = lz::generate_while([&i]() {
 *    auto copy = i++;
 *    return std::make_pair(copy, copy != 4);
 * }); // { 0, 1, 2, 3 }
 * // or (cxx 14)
 * auto generator = lz::generate_while([i = 0]() {
 *   auto pair = std::make_pair(i, i != 4);
 *    ++i;
 *   return pair;
 * }); // { 0, 1, 2, 3 }
 * ```
 *
 */
LZ_INLINE_VAR constexpr detail::generate_while_adaptor generate_while{};

/**
 * @brief Type alias helper for the generate_while iterable.
 * @tparam GeneratorFunc The type of the generator function.
 * ```cpp
 * int i = 0;
 * lz::generate_while_iterable<std::function<std::pair<bool, int>()>> generator([&i]() {
 *     auto copy = i++;
 *     return std::make_pair(copy, copy != 4);
 * });
 * ```
 */
template<class GeneratorFunc>
using generate_while_iterable = detail::generate_while_iterable<GeneratorFunc>;

} // namespace lz

#endif
