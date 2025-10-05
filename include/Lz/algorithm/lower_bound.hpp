#pragma once

#ifndef LZ_ALGORITHM_LOWER_BOUND_HPP
#define LZ_ALGORITHM_LOWER_BOUND_HPP

#include <Lz/detail/algorithm/lower_bound.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/procs/distance.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Searches for the first element in the partitioned range [begin, end) which is not ordered before @p value.
 *
 * @param begin The beginning of the range to check
 * @param end The end of the range to check
 * @param value The value to check for
 * @param binary_predicate The to use when comparing the values
 * @return Iteartor to the first element that satisfies the value or `end` if the element is not found
 */
template<class Iterator, class S, class T, class BinaryPredicate = detail::less>
LZ_CONSTEXPR_CXX_14 Iterator lower_bound(Iterator begin, S end, const T& value, BinaryPredicate&& binary_predicate = {}) {
    return detail::algorithm::sized_lower_bound(std::move(begin), value, std::forward<BinaryPredicate>(binary_predicate),
                                                lz::distance(begin, end));
}

/**
 * @brief Searches for the first element in the partitioned range [begin(iterable), end(iterable)) which is not ordered before
 * @p value.
 *
 * @param iterable The iterable to check
 * @param value The value to check for
 * @param binary_predicate The to use when comparing the values
 * @return Iteartor to the first element that satisfies the value or `end(iterable)` if the element is not found
 */
template<class Iterable, class T, class BinaryPredicate = detail::less>
LZ_CONSTEXPR_CXX_14 iter_t<Iterable> lower_bound(Iterable&& iterable, const T& value, BinaryPredicate&& binary_predicate = {}) {
    return detail::algorithm::lower_bound(std::forward<Iterable>(iterable), value,
                                          std::forward<BinaryPredicate>(binary_predicate));
}
} // namespace lz

#endif // LZ_ALGORITHM_LOWER_BOUND_HPP
