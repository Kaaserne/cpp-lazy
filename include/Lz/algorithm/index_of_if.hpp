#pragma once

#ifndef LZ_ALGORITHM_INDEX_OF_IF_HPP
#define LZ_ALGORITHM_INDEX_OF_IF_HPP

#include <Lz/detail/algorithm/index_of_if.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 size_t index_of_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    return lz::detail::algorithm::index_of_if(std::move(begin), std::move(end), std::move(unary_predicate));
}

/**
 * @brief Returns the index of the first element in the iterable that satisfies the unary_predicate @p unary_predicate.
 * If no such element is found, it returns lz::npos.
 * @param iterable The iterable to search.
 * @param unary_predicate The search unary_predicate in [begin(iterable), end(iterable)). Must return bool.
 * @return The index of the first element that satisfies the unary_predicate @p unary_predicate, or lz::npos if no such
 * element is found.
 */
template<class Iterable, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 size_t index_of_if(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return lz::index_of_if(detail::begin(iterable), detail::end(iterable), std::move(unary_predicate));
}

} // namespace lz

#endif
