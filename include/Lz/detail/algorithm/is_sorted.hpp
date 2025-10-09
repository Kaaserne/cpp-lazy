#pragma once

#ifndef LZ_DETAIL_ALGORITHM_IS_SORTED_HPP
#define LZ_DETAIL_ALGORITHM_IS_SORTED_HPP

#include <Lz/detail/traits/iterator_categories.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 bool is_sorted(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if constexpr (is_ra_v<Iterator>) {
        auto last = begin + (end - begin);
        return std::is_sorted(std::move(begin), std::move(last), std::move(binary_predicate));
    }
    else {
        if (begin == end) {
            return true;
        }
        auto next = begin;
        for (++next; next != end; ++begin, ++next) {
            if (binary_predicate(*next, *begin)) {
                return false;
            }
        }
        return true;
    }
}

#else

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator>::value, bool> is_sorted(Iterator begin, S end, BinaryPredicate binary_predicate) {
    auto last = begin + (end - begin);
    return std::is_sorted(std::move(begin), std::move(last), std::move(binary_predicate));
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value, bool>
is_sorted(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return true;
    }
    auto next = begin;
    for (++next; next != end; ++begin, ++next) {
        if (binary_predicate(*next, *begin)) {
            return false;
        }
    }
    return true;
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
