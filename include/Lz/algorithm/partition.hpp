#pragma once

#ifndef LZ_ALGORITHM_PARTITION_HPP
#define LZ_ALGORITHM_PARTITION_HPP

#include <Lz/detail/algorithm/partition.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        return detail::algorithm::partition(std::move(begin), std::move(end), std::move(unary_predicate));
    }
    else {
        return std::partition(std::move(begin), std::move(end), std::move(unary_predicate));
    }
}

#else

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_sentinel<Iterator, S>::value, Iterator>
partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    return detail::algorithm::partition(std::move(begin), std::move(end), std::move(unary_predicate));
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_sentinel<Iterator, S>::value, Iterator>
partition(Iterator begin, S end, UnaryPredicate unary_predicate) {
    return std::partition(std::move(begin), std::move(end), std::move(unary_predicate));
}

#endif

/**
 * @brief Partitions the iterable in such a way that all elements that satisfy the unary_predicate come before all elements
 * that do not satisfy the unary_predicate. The relative order of the elements is not preserved.
 *
 * @param iterable The iterable to partition
 * @param unary_predicate The unary_predicate to partition the iterable with
 * @return Iterator to the first element in the second group (the elements that do not satisfy the unary_predicate)
 */
template<class Iterable, class UnaryPredicate>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> partition(Iterable&& iterable, UnaryPredicate unary_predicate) {
    return lz::partition(detail::begin(iterable), detail::end(iterable), std::move(unary_predicate));
}

} // namespace lz

#endif
