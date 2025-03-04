#pragma once

#ifndef LZ_JOIN_WHERE_HPP
#define LZ_JOIN_WHERE_HPP

#include <Lz/basic_iterable.hpp>
#include <Lz/detail/adaptors/join_where.hpp>

namespace lz {
LZ_MODULE_EXPORT_SCOPE_BEGIN

#ifdef LZ_HAS_CXX_11

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
static constexpr detail::join_where_adaptor detail::join_where_adaptor::join_where{};

#else

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

#endif

LZ_MODULE_EXPORT_SCOPE_END

} // namespace lz

#endif // LZ_JOIN_WHERE_HPP
