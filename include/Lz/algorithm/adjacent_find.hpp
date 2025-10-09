#pragma once

#ifndef LZ_ALGORITHM_ADJACENT_FIND_HPP
#define LZ_ALGORITHM_ADJACENT_FIND_HPP

#include <Lz/detail/algorithm/adjacent_find.hpp>
#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/traits/iter_type.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

LZ_MODULE_EXPORT namespace lz {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class BinaryPredicate>
[[nodiscard]] constexpr Iterator adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if constexpr (detail::is_sentinel_v<Iterator, S>) {
        return detail::algorithm::adjacent_find(std::move(begin), std::move(end), std::move(binary_predicate));
    }
    else {
        return std::adjacent_find(std::move(begin), std::move(end), std::move(binary_predicate));
    }
}

#else

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<detail::is_sentinel<Iterator, S>::value, Iterator>
adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    return detail::algorithm::adjacent_find(std::move(begin), std::move(end), std::move(binary_predicate));
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 detail::enable_if_t<!detail::is_sentinel<Iterator, S>::value, Iterator>
adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    return std::adjacent_find(std::move(begin), std::move(end), std::move(binary_predicate));
}

#endif // LZ_HAS_CXX_17

/**
 * @brief Finds the first occurrence of two adjacent elements in the range [begin(iterable), end(iterable)) that satisfy the
 * binary binary_predicate @p binary_predicate
 *
 * @param iterable The iterable to search in
 * @param binary_predicate The binary binary_predicate to search with
 * @return Iterator to the first occurrence of two adjacent elements that satisfy the binary binary_predicate or
 * `end(iterable)` if the elements are not found
 */
template<class Iterable, class BinaryPredicate = detail::equal_to>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 iter_t<Iterable> adjacent_find(Iterable&& iterable, BinaryPredicate&& binary_predicate = {}) {
    return lz::adjacent_find(detail::begin(iterable), detail::end(iterable), std::forward<BinaryPredicate>(binary_predicate));
}

} // namespace lz

#endif
