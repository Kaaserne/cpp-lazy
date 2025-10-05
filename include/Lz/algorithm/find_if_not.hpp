#pragma once

#ifndef LZ_ALGORITHM_FIND_IF_NOT_HPP
#define LZ_ALGORITHM_FIND_IF_NOT_HPP

#include <Lz/algorithm/find_if.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator find_if_not(Iterator begin, S end, UnaryPredicate unary_predicate) {
    using std::find_if;
    return find_if(std::move(begin), std::move(end),
                   [&unary_predicate](detail::ref_t<Iterator> value) { return !unary_predicate(value); });
}

/**
 * @brief Finds the first element in the range [begin(iterable), end(iterable)) that does not satisfy the unary_predicate @p
 * unary_predicate
 *
 * @param iterable The iterable to find the element in
 * @param unary_predicate The unary_predicate to find the element with
 * @return Iterator to the first element that does not satisfy the unary_predicate or `end(iterable)` if the element is not
 * found
 */
template<class Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> find_if_not(Iterable&& iterable, UnaryPredicate&& unary_predicate) {
    using lz::find_if_not;
    using std::find_if_not;
    return find_if_not(detail::begin(iterable), detail::end(iterable), std::forward<UnaryPredicate>(unary_predicate));
}
} // namespace lz

#endif // LZ_ALGORITHM_FIND_IF_NOT_HPP
