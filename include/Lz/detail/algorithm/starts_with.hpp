#pragma once

#ifndef LZ_DETAIL_ALGORITHM_STARTS_WITH_HPP
#define LZ_DETAIL_ALGORITHM_STARTS_WITH_HPP

#include <Lz/detail/traits/is_sentinel.hpp>
#include <Lz/detail/traits/std_algo_compat.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

#include <algorithm>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
constexpr bool starts_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    if constexpr (is_ra_v<Iterator2> && is_ra_v<Iterator1>) {
        auto len = end2 - begin2;
        if ((end - begin) < len) {
            return false;
        }
        return std_equal_helper(begin2, end2, begin, begin + len, std::move(binary_predicate));
    }
    else {
        for (; begin2 != end2; ++begin, (void)++begin2) {
            if (begin == end || !binary_predicate(*begin, *begin2)) {
                return false;
            }
        }
        return true;
    }
}

#elif defined(LZ_HAS_CXX_14)

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator1>::value && is_ra<Iterator2>::value, bool>
starts_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    auto len = end2 - begin2;
    if ((end - begin) < len) {
        return false;
    }
    return std_equal_helper(begin2, end2, begin, begin + len, std::move(binary_predicate));
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPreidcate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator1>::value || !is_ra<Iterator2>::value, bool>
starts_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPreidcate binary_predicate) {
    for (; begin2 != end2; ++begin, (void)++begin2) {
        if (begin == end || !binary_predicate(*begin, *begin2)) {
            return false;
        }
    }
    return true;
}

#else

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator1>::value || !is_ra<Iterator2>::value, bool>
starts_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    for (; begin2 != end2; ++begin, (void)++begin2) {
        if (begin == end || !binary_predicate(*begin, *begin2)) {
            return false;
        }
    }
    return true;
}

template<class Iterator1, class S1, class Iterator2, class S2, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator1>::value && is_ra<Iterator2>::value, bool>
starts_with(Iterator1 begin, S1 end, Iterator2 begin2, S2 end2, BinaryPredicate binary_predicate) {
    auto len = end2 - begin2;
    if ((end - begin) < len) {
        return false;
    }
    return std_equal_helper(begin2, end2, begin, begin + len, std::move(binary_predicate));
}

#endif

} // namespace detail
} // namespace lz

#endif
