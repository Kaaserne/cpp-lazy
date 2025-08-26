#pragma once

#ifndef LZ_DROP_WHILE_HPP
#define LZ_DROP_WHILE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/drop_while.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief This adaptor is used to make an iterable where the iterator keeps dropping elements as long as the predicate returns
 * `true`. Once it has returned `false`, it will no longer do such thing. The iterator category is the same as its input iterable.
 * Its end() function will return a sentinel if its input iterable is forward or less or has a sentinel. Example:
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto dropped = lz::drop_while(vec, [](int i) { return i < 3; }); // dropped = { 3, 4, 5 }
 * // or
 * auto dropped = vec | lz::drop_while([](int i) { return i < 3; }); // dropped = { 3, 4, 5 }
 * ```
 */
LZ_INLINE_VAR constexpr detail::drop_while_adaptor drop_while{};

/**
 * @brief This is a type alias for the `drop_while` iterable.
 * @tparam Iterable The iterable to drop elements from.
 * @tparam UnaryPredicate The predicate to use for dropping elements.
 * ```cpp
 * std::vector<int> vec = { 1, 2, 3, 4, 5 };
 * auto f = [](int i) { return i < 3; };
 * lz::drop_while_iterable<std::vector<int>, std::function<bool(int)> dropped = lz::drop_while(vec, f);
 */
template<class Iterable, class UnaryPredicate>
using drop_while_iterable = detail::drop_while_iterable<Iterable, UnaryPredicate>;

} // namespace lz

#endif // LZ_DROP_WHILE_HPP
