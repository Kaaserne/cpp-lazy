#pragma once

#ifndef LZ_DETAIL_ALGORITHM_MAX_ELEMENT_HPP
#define LZ_DETAIL_ALGORITHM_MAX_ELEMENT_HPP

#include <Lz/detail/traits/iterator_categories.hpp>
#include <algorithm>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator max_element(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if constexpr (is_ra_v<Iterator>) { //  TODO enable_if
        auto common_end = begin + (end - begin);
        return std::max_element(std::move(begin), std::move(common_end), std::move(binary_predicate));
    }
    else {
        if (begin == end) {
            return begin;
        }
        auto max = begin;
        for (++begin; begin != end; ++begin) {
            if (binary_predicate(*max, *begin)) {
                max = begin;
            }
        }
        return max;
    }
}

#else

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator>::value, Iterator>
max_element(Iterator begin, S end, BinaryPredicate binary_predicate) {
    auto common_end = begin + (end - begin);
    return std::max_element(std::move(begin), std::move(common_end), std::move(binary_predicate));
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value, Iterator>
max_element(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {
        return begin;
    }
    auto max = begin;
    for (++begin; begin != end; ++begin) {
        if (binary_predicate(*max, *begin)) {
            max = begin;
        }
    }
    return max;
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
