#pragma once

#ifndef LZ_ALGORITHM_BINARY_SEARCH_HPP
#define LZ_ALGORITHM_BINARY_SEARCH_HPP

#include <Lz/algorithm/lower_bound.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator, class S, class T, class BinaryPredicate = detail::less>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool
binary_search(Iterator begin, S end, const T& value, BinaryPredicate binary_predicate = {}) {
    using lz::lower_bound;
    using std::lower_bound;
    auto it = lower_bound(std::move(begin), std::move(end), value, binary_predicate);
    return it != end && !binary_predicate(value, *it);
}

/**
 * @brief Searches for the first occurrence of the value @p value in the range [begin(iterable), end(iterable)) using the
 * binary search algorithm. @p iterable must be sorted beforehand.
 * @param iterable The iterable to search in
 * @param value The value to search for
 * @param binary_predicate Predicate to search the value with
 * @return true if the value is found, false otherwise
 */
template<class Iterable, class T, class BinaryPredicate = detail::less>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool binary_search(Iterable&& iterable, const T& value, BinaryPredicate binary_predicate = {}) {
    auto end = detail::end(iterable);
    auto it = lz::lower_bound(std::forward<Iterable>(iterable), value, binary_predicate);
    return it != end && !binary_predicate(value, *it);
}
} // namespace lz

#endif // LZ_ALGORITHM_BINARY_SEARCH_HPP
