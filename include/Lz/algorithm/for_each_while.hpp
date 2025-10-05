#pragma once

#ifndef LZ_ALGORITHM_FOR_EACH_WHILE_HPP
#define LZ_ALGORITHM_FOR_EACH_WHILE_HPP

#include <Lz/detail/algorithm/for_each_while.hpp>
#include <Lz/detail/procs/begin_end.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 void for_each_while(Iterator begin, S end, UnaryPredicate unary_predicate) {
    return detail::algorithm::for_each_while(std::move(begin), std::move(end), std::move(unary_predicate));
}

/**
 * @brief Applies the given function to each element in the iterable until the function returns false. @p unary_predicate
 * should return a boolean. If `true` is returned, the next element will be processed. If `false` is returned, the iteration
 * stops.
 *
 * @param iterable The iterable to process.
 * @param unary_predicate The function to apply to each element. Should return a boolean.
 */
template<class Iterable, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 void for_each_while(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return for_each_while(detail::begin(std::forward<Iterable>(iterable)), detail::end(std::forward<Iterable>(iterable)),
                          std::move(unary_predicate));
}
} // namespace lz

#endif
