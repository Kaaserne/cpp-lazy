#pragma once

#ifndef LZ_DETAIL_ALGORITHM_MISMATCH_HPP
#define LZ_DETAIL_ALGORITHM_MISMATCH_HPP

#include <Lz/detail/procs/operators.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <algorithm>
#include <utility>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
constexpr std::pair<Iterator, Iterator2>
mismatch(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    if constexpr (is_ra_v<Iterator> && is_ra_v<Iterator2>) {
        auto last1 = begin + (end - begin);
        auto last2 = begin2 + (end2 - begin2);
        return std::mismatch(std::move(begin), std::move(last1), std::move(begin2), std::move(last2),
                             std::move(binary_predicate));
    }
    else {
        while (begin != end && begin2 != end2 && binary_predicate(*begin, *begin2)) {
            ++begin, ++begin2;
        }
        return std::make_pair(begin, begin2);
    }
}

#elif defined(LZ_HAS_CXX_14)

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator>::value && is_ra<Iterator2>::value, std::pair<Iterator, Iterator2>>
mismatch(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    auto last1 = begin + (end - begin);
    auto last2 = begin2 + (end2 - begin2);
    return std::mismatch(std::move(begin), std::move(last1), std::move(begin2), std::move(last2), std::move(binary_predicate));
}

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value || !is_ra<Iterator2>::value, std::pair<Iterator, Iterator2>>
mismatch(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    while (begin != end && begin2 != end2 && binary_predicate(*begin, *begin2)) {
        ++begin, ++begin2;
    }
    return std::make_pair(begin, begin2);
}

#else

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator>::value && is_ra<Iterator2>::value, std::pair<Iterator, Iterator2>>
mismatch(Iterator begin, S end, Iterator2 begin2, S2, BinaryPredicate binary_predicate) {
    auto last1 = begin + (end - begin);
    return std::mismatch(std::move(begin), std::move(last1), std::move(begin2), std::move(binary_predicate));
}

template<class Iterator, class S, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value || !is_ra<Iterator2>::value, std::pair<Iterator, Iterator2>>
mismatch(Iterator begin, S end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    while (begin != end && begin2 != end2 && binary_predicate(*begin, *begin2)) {
        ++begin, ++begin2;
    }
    return std::make_pair(begin, begin2);
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_MISMATCH_HPP
