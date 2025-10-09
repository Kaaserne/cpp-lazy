#pragma once

#ifndef LZ_DETAIL_ALGORITHM_COUNT_IF_HPP
#define LZ_DETAIL_ALGORITHM_COUNT_IF_HPP

#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <algorithm>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class UnaryPredicate>
constexpr diff_type<Iterator> count_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    if constexpr (is_ra_v<Iterator>) {
        auto last = begin + (end - begin);
        return std::count_if(std::move(begin), std::move(last), std::move(unary_predicate));
    }
    else {
        diff_type<Iterator> count = 0;
        for (; begin != end; ++begin) {
            if (unary_predicate(*begin)) {
                ++count;
            }
        }
        return count;
    }
}

#else

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<is_ra<Iterator>::value, diff_type<Iterator>> count_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    auto last = begin + (end - begin);
    return std::count_if(std::move(begin), std::move(last), std::move(unary_predicate));
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!is_ra<Iterator>::value, diff_type<Iterator>> count_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    diff_type<Iterator> count = 0;
    for (; begin != end; ++begin) {
        if (unary_predicate(*begin)) {
            ++count;
        }
    }
    return count;
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz
#endif // LZ_DETAIL_ALGORITHM_COUNT_IF_HPP
