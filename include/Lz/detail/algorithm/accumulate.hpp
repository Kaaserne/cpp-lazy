#pragma once

#ifndef LZ_DETAIL_ALGORITHM_ACCUMULATE_HPP
#define LZ_DETAIL_ALGORITHM_ACCUMULATE_HPP

#include <Lz/detail/compiler_checks.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/std_algo_compat.hpp>

#ifdef LZ_HAS_CXX_20
#include <Lz/detail/procs/get_end.hpp>
#include <numeric>
#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_20

template<class Iterator, class S, class T, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 T accumulate(Iterator begin, S end, T init, UnaryPredicate unary_predicate) {
    if constexpr (std_algo_compat_v<Iterator, S>) {
        return std::accumulate(begin, detail::get_end(begin, end), std::move(init), std::move(unary_predicate));
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

} // namespace detail
} // namespace lz

#endif // LZ_DETAIL_ALGORITHM_ACCUMULATE_HPP
