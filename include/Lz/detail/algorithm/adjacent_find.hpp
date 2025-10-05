#pragma once

#ifndef LZ_DETAIL_ALGORITHM_ADJACENT_FIND_HPP
#define LZ_DETAIL_ALGORITHM_ADJACENT_FIND_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

#include <algorithm>

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class BinaryPredicate>
constexpr Iterator adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if constexpr (is_ra_v<Iterator>) {
        auto last = begin + (end - begin);
        return std::adjacent_find(std::move(begin), std::move(last), std::move(binary_predicate));
    }
    else {
        if (begin == end) {
            return begin;
        }
        auto next = begin;
        for (++next; next != end; ++begin, ++next) {
            if (binary_predicate(*begin, *next)) {
                return begin;
            }
        }
        return next;
    }
}

#else
template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value, Iterator>
adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    if (begin == end) {

        return begin;
    }

    auto next = begin;
    for (++next; next != end; ++begin, ++next) {
        if (binary_predicate(*begin, *next)) {
            return begin;
        }
    }
    return next;
}

template<class Iterator, class S, class BinaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator>::value, Iterator>
adjacent_find(Iterator begin, S end, BinaryPredicate binary_predicate) {
    auto last = begin + (end - begin);
    return std::adjacent_find(std::move(begin), std::move(last), std::move(binary_predicate));
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
