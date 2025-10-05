#pragma once

#ifndef LZ_ALGORITHM_MISMATCH_HPP
#define LZ_ALGORITHM_MISMATCH_HPP

#include <Lz/detail/algorithm/mismatch.hpp>
#include <Lz/detail/traits/is_sentinel.hpp>

LZ_MODULE_EXPORT namespace lz {

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate = detail::equal_to>
LZ_CONSTEXPR_CXX_14 std::pair<Iterator1, Iterator2>
mismatch(Iterator1 begin1, S1 end1, Iterator2 begin2, S2 end2, BinaryPredicate&& binary_predicate = {}) {
    if constexpr (detail::is_sentinel_v<Iterator1, S1> || detail::is_sentinel_v<Iterator2, S2>) {
        return detail::algorithm::mismatch(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2),
                                           std::forward<BinaryPredicate>(binary_predicate));
    }
    else {
        return std::mismatch(std::move(begin1), std::move(end1), std::move(begin2), std::move(end2),
                             std::forward<BinaryPredicate>(binary_predicate));
    }
}

template<class Iterable1, class Iterable2, class BinaryPredicate = detail::equal_to>
LZ_NODISCARD LZ_CONSTEXPR_CXX_14 bool
mismatch(Iterable1&& iterable1, Iterable2&& iterable2, BinaryPredicate&& binary_predicate = {}) {
    using lz::mismatch;
    using std::mismatch;
    return mismatch(detail::begin(iterable1), detail::end(iterable1), detail::begin(iterable2), detail::end(iterable2),
                    std::forward<BinaryPredicate>(binary_predicate));
}
} // namespace lz

#endif
