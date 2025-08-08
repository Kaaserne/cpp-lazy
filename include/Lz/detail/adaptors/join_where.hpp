#pragma once

#ifndef LZ_JOIN_WHERE_ADAPTOR_HPP
#define LZ_JOIN_WHERE_ADAPTOR_HPP

#include <Lz/detail/adaptors/fn_args_holder.hpp>
#include <Lz/detail/concepts.hpp>
#include <Lz/detail/iterables/join_where.hpp>

namespace lz {
namespace detail {
struct join_where_adaptor {
    using adaptor = join_where_adaptor;

    /**
     * @brief Performs an SQL-like join on two iterables where the join condition is specified by the selectors. Its end()
     * function returns a sentinel, it contains a forward iterator. It also does not contain a size() method. The second iterable
     * must be sorted first. It is therfore recommended to sort the smallest iterable, performance wise. Operator< is used to
     * compare the first and the second selector. For instance, in the example below operator< is used to compare int with int.
     * Example:
     * ```cpp
     * std::vector<std::pair<int, int>> vec = {{1, 2}, {2, 3}, {3, 4}, {4, 5}};
     * std::vector<std::pair<int, int>> vec2 = {{1, 2}, {3, 4}, {4, 5}, {5, 6}};
     * auto joined = lz::join_where(vec, vec2,
     *                              [](const auto& a) { return a.first; }, // join on the first element of the pair in vec
     *                              [](const auto& a) { return a.first; }, // join on the first element of the pair in vec2
     *                              [](const auto& a, const auto& b) { return std::make_pair(a.first, b.second); }
     * ); // joined contains: {{1, 2}, {3, 4}, {4, 5}}
     * ```
     * @param iterable_a The first iterable to join on
     * @param iterable_b The second iterable to join on
     * @param a The selector for the first iterable, this is the value that will be compared with the second selector
     * @param b The selector for the second iterable, this is the value that will be compared with the first selector
     * @param result_selector The result selector, this is the value that will be returned when the join condition (`a(*iter_a) <
     * b(*iter_b)`) is met
     * @return A join_where_iterable that can be used to iterate over the joined elements
     */
    template<class IterableA, class IterableB, class SelectorA, class SelectorB, class ResultSelector>
    LZ_NODISCARD constexpr join_where_iterable<remove_ref<IterableA>, remove_ref<IterableB>, SelectorA, SelectorB, ResultSelector>
    operator()(IterableA&& iterable_a, IterableB&& iterable_b, SelectorA a, SelectorB b, ResultSelector result_selector) const {
        return { std::forward<IterableA>(iterable_a), std::forward<IterableB>(iterable_b), std::move(a), std::move(b),
                 std::move(result_selector) };
    }

    /**
     * @brief Performs an SQL-like join on two iterables where the join condition is specified by the selectors. Its end()
     * function returns a sentinel, it contains a forward iterator. It also does not contain a size() method. The second iterable
     * must be sorted first. It is therfore recommended to sort the smallest iterable, performance wise. Operator< is used to
     * compare the first and the second selector. For instance, in the example below operator< is used to compare int with int.
     * Example:
     * ```cpp
     * std::vector<std::pair<int, int>> vec = {{1, 2}, {2, 3}, {3, 4}, {4, 5}};
     * std::vector<std::pair<int, int>> vec2 = {{1, 2}, {3, 4}, {4, 5}, {5, 6}};
     * auto joined = vec | lz::join_where(vec2,
     *                                   [](const auto& a) { return a.first; }, // join on the first element of the pair in vec
     *                                   [](const auto& a) { return a.first; }, // join on the first element of the pair in vec2
     *                                   [](const auto& a, const auto& b) { return std::make_pair(a.first, b.second); }
     * ); // joined contains: {{1, 2}, {3, 4}, {4, 5}}
     * ```
     * @param iterable_b The second iterable to join on
     * @param a The selector for the first iterable, this is the value that will be compared with the second selector
     * @param b The selector for the second iterable, this is the value that will be compared with the first selector
     * @param result_selector The result selector, this is the value that will be returned when the join condition (`a(*iter_a) <
     * b(*iter_b)`) is met
     * @return An adaptor that can be used in pipe expressions
     */
    template<class IterableB, class SelectorA, class SelectorB, class ResultSelector>
    LZ_NODISCARD LZ_CONSTEXPR_CXX_14 fn_args_holder<adaptor, IterableB, SelectorA, SelectorB, ResultSelector>
    operator()(IterableB&& iterable_b, SelectorA a, SelectorB b, ResultSelector result_selector) const {
        return { std::forward<IterableB>(iterable_b), std::move(a), std::move(b), std::move(result_selector) };
    }
};
} // namespace detail
} // namespace lz

#endif
