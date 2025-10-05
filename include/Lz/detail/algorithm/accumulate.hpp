#pragma once

#ifndef LZ_DETAIL_ALGORITHM_ACCUMULATE_HPP
#define LZ_DETAIL_ALGORITHM_ACCUMULATE_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>

#ifdef LZ_HAS_CXX_20
#include <numeric>
#endif

namespace lz {
namespace detail {
namespace algorithm {

#ifdef LZ_HAS_CXX_20

template<class Iterator, class S, class T, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 T accumulate(Iterator begin, S end, T init, UnaryPredicate unary_predicate) {
    if constexpr (is_ra_v<Iterator>) {
        auto common_end = begin + (end - begin);
        return std::accumulate(std::move(begin), std::move(common_end), std::move(init), std::move(unary_predicate));
    }
    else {
        for (; begin != end; ++begin) {
            init = unary_predicate(std::move(init), *begin);
        }
        return init;
    }
}

#else

template<class Iterator, class S, class T, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 T accumulate(Iterator begin, S end, T init, UnaryPredicate unary_predicate) {
    for (; begin != end; ++begin) {
        init = unary_predicate(std::move(init), *begin);
    }
    return init;
}

#endif

} // namespace algorithm
} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_ACCUMULATE_HPP
