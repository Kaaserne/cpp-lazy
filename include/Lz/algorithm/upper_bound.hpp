#pragma once

#ifndef LZ_ALGORITHM_UPPER_BOUND_HPP
#define LZ_ALGORITHM_UPPER_BOUND_HPP

#include <Lz/algorithm/lower_bound.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class T, class BinaryPredicate = detail::less>
LZ_CONSTEXPR_CXX_14 Iterator upper_bound(Iterator begin, S end, const T& value, BinaryPredicate&& binary_predicate) {
    return lz::lower_bound(std::move(begin), std::move(end), value,
                           [&binary_predicate](detail::ref_t<Iterator> v1, const T& v2) { return !binary_predicate(v2, v1); });
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
LZ_CONSTEXPR_CXX_14 iter_t<Iterable> upper_bound(Iterable&& iterable, const T& value, BinaryPredicate&& binary_predicate = {}) {
    return lz::lower_bound(
        std::forward<Iterable>(iterable), value,
        [&binary_predicate](detail::ref_iterable_t<Iterable> v1, const T& v2) { return !binary_predicate(v2, v1); });
}
} // namespace lz

#endif // LZ_ALGORITHM_UPPER_BOUND_HPP
