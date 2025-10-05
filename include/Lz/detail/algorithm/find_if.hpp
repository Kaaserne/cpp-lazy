#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FIND_IF_HPP
#define LZ_DETAIL_ALGORITHM_FIND_IF_HPP

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

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    if constexpr (is_ra_v<Iterator>) {
        auto common_end = begin + (end - begin);
        return std::find_if(std::move(begin), std::move(common_end), std::move(unary_predicate));
    }
    else {
        for (; begin != end; ++begin) {
            if (unary_predicate(*begin)) {
                break;
            }
        }
        return begin;
    }
}

#else

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if<is_ra<Iterator>::value, Iterator> find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    auto common_end = begin + (end - begin);
    return std::find_if(std::move(begin), std::move(common_end), std::move(unary_predicate));
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if<!is_ra<Iterator>::value, Iterator> find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    for (; begin != end; ++begin) {
        if (unary_predicate(*begin)) {
            break;
        }
    }
    return begin;
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif
