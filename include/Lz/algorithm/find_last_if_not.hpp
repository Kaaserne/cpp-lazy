#pragma once

#ifndef LZ_ALGORITHM_FIND_LAST_IF_NOT_HPP
#define LZ_ALGORITHM_FIND_LAST_IF_NOT_HPP

#include <Lz/algorithm/find_last_if.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator find_last_if_not(Iterator begin, S end, UnaryPredicate unary_predicate) {
    return lz::find_last_if(std::forward<Iterator>(begin), std::forward<S>(end),
                            [&unary_predicate](detail::ref_t<Iterator> value) { return !unary_predicate(value); });
}

/**
 * @brief Finds the last element in the range [begin(iterable), end(iterable)) that does not satisfy the unary_predicate @p
 * unary_predicate. Does not use `lz::cached_reverse` to reverse the elements (if bidirectional).
 *
 * @param iterable The iterable to find the element in
 * @param unary_predicate The unary_predicate to find the element with
 * @return Iterator to the last element that does not satisfy the unary_predicate or `end(iterable)` if the element is not
 * found
 */
template<class Iterable, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 iter_t<Iterable>
find_last_if_not(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return lz::find_last_if_not(detail::begin(iterable), detail::end(iterable), std::move(unary_predicate));
}

} // namespace lz

#endif // LZ_ALGORITHM_FIND_LAST_IF_NOT_HPP
