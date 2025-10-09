#pragma once

#ifndef LZ_ALGORITHM_FOR_EACH_WHILE_N_HPP
#define LZ_ALGORITHM_FOR_EACH_WHILE_N_HPP

#include <Lz/detail/algorithm/for_each_while_n.hpp>
#include <Lz/detail/procs/begin_end.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void for_each_while_n(Iterator begin, S end, size_t n, UnaryOp unary_op) {
    detail::algorithm::for_each_while_n(std::move(begin), std::move(end), n, std::move(unary_op));
}

/**
 * @brief Keeps iterating over @p iterable while @p binary_predicate returns true and @p n < distance(begin(iterable),
 * end(iterable)).
 *
 * @param iterable A range
 * @param n The amount of elements to iterate over
 * @param unary_op Predicate that must either return true or false
 */
template<class Iterable, class UnaryOp>
LZ_CONSTEXPR_CXX_14 void for_each_while_n(Iterable&& iterable, size_t n, UnaryOp unary_op) {
    lz::for_each_while_n(detail::begin(iterable), detail::end(iterable), n, std::move(unary_op));
}

} // namespace lz

#endif // LZ_ALGORITHM_FOR_EACH_WHILE_N_HPP
