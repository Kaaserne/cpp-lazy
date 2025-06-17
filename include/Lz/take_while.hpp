#pragma once

#ifndef LZ_TAKE_WHILE_HPP
#define LZ_TAKE_WHILE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/take_while.hpp>

namespace lz {

LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Takes elements from an iterable while the given predicte returns `true`. Will return a bidirectional iterable if the
 * given iterable is at least bidirectional. Does not contain a size() method and if its input iterable is forward or has a
 * sentinel, will return a default_sentinel. Example:
 * ```cpp
 * std::vector<int> vec = {1, 2, 3, 4, 5};
 * auto take_while = lz::take_while(vec, [](int i) { return i < 3; }); // {1, 2}
 * // or
 * auto take_while = vec | lz::take_while([](int i) { return i < 3; }); // {1, 2}
 * ```
 */
LZ_INLINE_VAR constexpr detail::take_while_adaptor take_while{};

/**
 * @brief Take while iterable helper alias.
 * @tparam Iterable The type of the iterable to take from.
 * @tparam UnaryPredicate The type of the unary predicate to use.
 * ```cpp
 * std::vector<int> vec = {1, 2, 3, 4, 5};
 * using take_while = lz::take_while_iterable<std::vector<int>, std::function<bool(int)>>;
 * take_while take_while_iter = lz::take_while(vec, [](int i) { return i < 3; });
 * ```
 */
template<class Iterable, class UnaryPredicate>
using take_while_iterable = detail::take_while_iterable<Iterable, UnaryPredicate>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_TAKE_WHILE_HPP