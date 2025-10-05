#pragma once

#ifndef LZ_ALGORITHM_SEARCH_HPP
#define LZ_ALGORITHM_SEARCH_HPP

#include <Lz/detail/algorithm/search.hpp>
#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>

LZ_MODULE_EXPORT namespace lz {

/**
 * @brief Searches for the first occurrence of the range [begin2, end2) in the range [begin, end)
 *
 * @param begin The beginning of the range to search in
 * @param end The end of the range to search in
 * @param begin2 The beginning of the range to search for
 * @param end2 The end of the range to search for
 * @param binary_predicate The binary predicate to search with
 * @return The iterator to the first occurrence of the range [begin(iterable2), end(iterable2)) in the range [begin(iterable),
 * end(iterable)) or `end(iterable)` if the range is not found
 */
template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::pair<Iterator, Iterator>
search(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate&& binary_predicate = {}) {
    return detail::algorithm::search(std::move(begin), std::move(end), std::move(begin2), std::move(end2),
                                     std::forward<BinaryPredicate>(binary_predicate));
}

/**
 * @brief Searches for the first occurrence of the range [begin(iterable2), end(iterable2)) in the range [begin(iterable),
 * end(iterable))
 *
 * @param iterable The iterable to search in
 * @param iterable2 The iterable to search for
 * @param binary_predicate The binary predicate to search with
 * @return The iterator to the first occurrence of the range [begin(iterable2), end(iterable2)) in the range [begin(iterable),
 * end(iterable)) or `end(iterable)` if the range is not found
 */
template<class Iterable, class Iterable2, class BinaryPredicate = detail::equal_to>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 std::pair<iter_t<Iterable>, iter_t<Iterable>>
search(Iterable&& iterable, Iterable2&& iterable2, BinaryPredicate&& binary_predicate = {}) {
    return lz::search(detail::begin(iterable), detail::end(iterable), detail::begin(iterable2), detail::end(iterable2),
                      std::forward<BinaryPredicate>(binary_predicate));
}

} // namespace lz

#endif
