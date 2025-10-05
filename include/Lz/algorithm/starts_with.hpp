#pragma once

#ifndef LZ_ALGORITHM_STARTS_WITH_HPP
#define LZ_ALGORITHM_STARTS_WITH_HPP

#include <Lz/algorithm/mismatch.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool starts_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate&& binary_predicate) {
    if constexpr (detail::is_sentinel_v<Iterator1, S1> && detail::is_sentinel_v<Iterator2, S2>) {
        const auto mm = lz::mismatch(std::move(begin), std::move(end), std::move(begin2), std::move(end2),
                                     std::forward<BinaryPredicate>(binary_predicate));
        return mm.second == end2;
    }
    else {
        const auto mm = std::mismatch(std::move(begin), std::move(end), std::move(begin2), std::move(end2),
                                      std::forward<BinaryPredicate>(binary_predicate));
        return mm.second == end2;
    }
}

/**
 * @brief Checks if the beginning of the first iterable matches the second iterable, using an optional binary predicate for
 * comparison.
 *
 * @param iterable The first iterable to check.
 * @param iterable2 The second iterable to compare against.
 * @param binary_predicate A binary predicate that takes two elements (one from each iterable) and returns true if they are
 * considered equal. Defaults to `std::equal_to<>`.
 * @return true if the beginning of the first iterable matches the second iterable, false otherwise.
 */
template<class Iterable, class Iterable2, class BinaryPredicate = detail::equal_to>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool
starts_with(Iterable&& iterable, Iterable2&& iterable2, BinaryPredicate&& binary_predicate = {}) {
    return starts_with(detail::begin(iterable), detail::end(iterable), detail::begin(iterable2), detail::end(iterable2),
                       std::forward<BinaryPredicate>(binary_predicate));
}
} // namespace lz

#endif
