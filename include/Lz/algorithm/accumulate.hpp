#pragma once

#ifndef LZ_ALGORITHM_ACCUMULATE_HPP
#define LZ_ALGORITHM_ACCUMULATE_HPP

#include <Lz/detail/algorithm/accumulate.hpp>

#ifdef LZ_HAS_CXX_20
#include <numeric>
#endif

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_20

template<class Iterator, class S, class T, class UnaryPredicate>
[[nodiscard]] constexpr Iterator accumulate(Iterator begin, S end, T init, UnaryPredicate unary_predicate) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        return detail::algorithm::accumulate(std::move(begin), std::move(end), std::move(init), std::move(unary_predicate));
    }
    else {
        return std::accumulate(std::move(begin), std::move(end), std::move(init), std::move(unary_predicate));
    }
}

#else

/**
 * @brief Accumulates the values in the range [begin, end) using the binary operator @p unary_predicate
 *
 * @param begin The beginning of the range to accumulate
 * @param end The end of the range to accumulate
 * @param init The initial value to start accumulating with
 * @param unary_predicate The binary operator to accumulate the values with
 * @return The accumulated value
 */
template<class Iterator, class S, class T, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 T accumulate(Iterator begin, S end, T init, UnaryPredicate unary_predicate) {
    return detail::algorithm::accumulate(std::move(begin), std::move(end), std::move(init), std::move(unary_predicate));
}

/**
 * @brief Accumulates the values in the range [begin, end) using the binary operator @p unary_predicate
 *
 * @param iterable The iterable to accumulate
 * @param init The initial value to start accumulating with
 * @param unary_predicate The binary operator to accumulate the values with
 * @return The accumulated value
 */
template<class Iterable, class T, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 T accumulate(Iterable&& iterable, T init, UnaryPredicate unary_predicate) {
    return lz::accumulate(detail::begin(iterable), detail::end(iterable), std::move(init), std::move(unary_predicate));
}

#endif
} // namespace lz

#endif
