#pragma once

#ifndef LZ_DETAIL_ALGORITHM_FIND_IF_HPP
#define LZ_DETAIL_ALGORITHM_FIND_IF_HPP

#include <Lz/detail/compiler_config.hpp>
#include <Lz/detail/procs/get_end.hpp>
#include <Lz/detail/traits/iterator_categories.hpp>
#include <Lz/detail/traits/std_algo_compat.hpp>

#ifndef LZ_HAS_CXX_17
#include <Lz/detail/traits/enable_if.hpp>
#endif

#include <algorithm>

namespace lz {
namespace detail {

#ifdef LZ_HAS_CXX_17

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 Iterator find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    if constexpr (std_algo_compat_v<Iterator, S>) {
        return std::find_if(begin, detail::get_end(begin, end), std::move(unary_predicate));
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
LZ_CONSTEXPR_CXX_14 enable_if_t<std_algo_compat<Iterator, S>::value, Iterator>
find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    return std::find_if(begin, detail::get_end(begin, end), std::move(unary_predicate));
}

template<class Iterator, class S, class UnaryPredicate>
LZ_CONSTEXPR_CXX_14 enable_if_t<!std_algo_compat<Iterator, S>::value, Iterator>
find_if(Iterator begin, S end, UnaryPredicate unary_predicate) {
    for (; begin != end; ++begin) {
        if (unary_predicate(*begin)) {
            break;
        }
    }
    return begin;
}

#endif

} // namespace detail
} // namespace lz

#endif
