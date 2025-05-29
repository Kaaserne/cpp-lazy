#pragma once

#ifndef LZ_JOIN_WHERE_HPP
#define LZ_JOIN_WHERE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/join_where.hpp>

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

/**
 * @brief Performs an SQL-like join on two iterables where the join condition is specified by the selectors. Its end() function
 * returns a sentinel, it contains a forward iterator. It also does not contain a size() method. The second iterable must be
 * sorted first. It is therfore recommended to sort the smallest iterable, performance wise. Operator< is used to compare the
 * first and the second selector. For instance, in the example below operator< is used to compare int with int. Example:
 * ```cpp
 * std::vector<std::pair<int, int>> vec = {{1, 2}, {2, 3}, {3, 4}, {4, 5}};
 * std::vector<std::pair<int, int>> vec2 = {{1, 2}, {3, 4}, {4, 5}, {5, 6}};
 * auto joined = lz::join_where(vec, vec2,
 *                              [](const auto& a) { return a.first; }, // join on the first element of the pair in vec
 *                              [](const auto& a) { return a.first; }, // join on the first element of the pair in vec2
 *                              [](const auto& a, const auto& b) { return std::make_pair(a.first, b.second); }
 * ); // joined contains: {{1, 2}, {3, 4}, {4, 5}}
 * // or
 * auto joined = vec | lz::join_where(vec2,
 *                                   [](const auto& a) { return a.first; }, // join on the first element of the pair in vec
 *                                   [](const auto& a) { return a.first; }, // join on the first element of the pair in vec2
 *                                   [](const auto& a, const auto& b) { return std::make_pair(a.first, b.second); }
 * ); // joined contains: {{1, 2}, {3, 4}, {4, 5}}
 * ```
 */
LZ_INLINE_VAR constexpr detail::join_where_adaptor join_where{};

/**
 * @brief Join where iterable helper alias.
 *
 * @tparam IterableA The first iterable type.
 * @tparam IterableB The second iterable type.
 * @tparam SelectorA The selector type for the first iterable.
 * @tparam SelectorB The selector type for the second iterable.
 * @tparam ResultSelector The result selector type that combines elements from both iterables.
 * ```cpp
 * std::vector<std::pair<int, int>> vec = {{1, 2}, {2, 3}, {3, 4}, {4, 5}};
 * std::vector<std::pair<int, int>> vec2 = {{1, 2}, {3, 4}, {4, 5}, {5, 6}};
 *
 * using it_a = std::vector<std::pair<int, int>>;
 * using it_b = std::vector<std::pair<int, int>>;
 * using selector_a = std::function<int(const std::pair<int, int>&)>;
 * using selector_b = std::function<int(const std::pair<int, int>&)>;
 * using result_selector = std::function<std::pair<int, int>(const std::pair<int, int>&, const std::pair<int, int>&)>;
 * using iterable = lz::join_where_iterable<it_a, it_b, selector_a, selector_b, result_selector>;
 *
 * iterable joined = lz::join_where_iterable(vec, vec2,
 *                             [](const auto& a) { return a.first; },
 *                             [](const auto& a) { return a.first; },
 *                             [](const auto& a, const auto& b) { return std::make_pair(a.first, b.second); }
 * );
 * ```
 */
template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector>
using join_where_iterable = detail::join_where_iterable<IterableA, IterableB, SelectorA, SelectorB, ResultSelector>;

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_JOIN_WHERE_HPP
