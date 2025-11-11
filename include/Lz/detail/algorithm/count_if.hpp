#pragma once

#ifndef LZ_DETAIL_ALGORITHM_COUNT_IF_HPP
#define LZ_DETAIL_ALGORITHM_COUNT_IF_HPP

#include <Lz/detail/procs/get_end.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/std_algo_compat.hpp>
#include <Lz/detail/traits/strict_iterator_traits.hpp>
#include <algorithm>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class UnaryPredicate>
constexpr diff_type<Iterator> count_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    if constexpr (std_algo_compat_v<Iterator, S>) {
        return std::count_if(begin, detail::get_end(begin, end), std::move(unary_predicate));
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
LZ_CONSTEXPR_CXX_14 enable_if_t<std_algo_compat<Iterator, S>::value, diff_type<Iterator>>
count_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    return std::count_if(begin, detail::get_end(begin, end), std::move(unary_predicate));
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat<Iterator, S>::value, diff_type<Iterator>>
count_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    diff_type<Iterator> count = 0;
    for (; begin != end; ++begin) {
        if (unary_predicate(*begin)) {
            ++count;
        }
    }
    return count;
}

#endif

} // namespace detail
} // namespace lz
#endif // LZ_DETAIL_ALGORITHM_COUNT_IF_HPP
